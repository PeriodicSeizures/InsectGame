#include <iostream>
#include "tcp_connection.h"

Side TCPConnection::SIDE;
AsyncQueue<OwnedPacket>* TCPConnection::in_packets_s;
AsyncQueue<Packet>* TCPConnection::in_packets_c;

void TCPConnection::init(AsyncQueue<OwnedPacket>* in_packets_s,
	AsyncQueue<Packet>* in_packets_c) 
{
	assert(in_packets_s || in_packets_c, "packet queue not assigned\n");

	if (in_packets_s)
		SIDE = Side::SERVER;
	else if (in_packets_c) SIDE = Side::CLIENT;

	TCPConnection::in_packets_s = in_packets_s;
	TCPConnection::in_packets_c = in_packets_c;
}

// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/example/cpp11/ssl/client.cpp
TCPConnection::TCPConnection(asio::io_context& ctx, ssl_socket socket)
	: _socket(std::move(socket)),
	ping_timer(ctx),
	pong_timer(ctx)
{
	_socket.set_verify_mode(asio::ssl::verify_none);
}

TCPConnection::~TCPConnection() {
	LOG_DEBUG("TCPConnection::deconstructor()\n");
}

ssl_socket& TCPConnection::socket() {
	return _socket;
}

bool TCPConnection::is_open() {
	return open;
}

void TCPConnection::close() {
	open = false;

	asio::error_code ec;
	_socket.lowest_layer().close(ec);

	pong_timer.cancel();
}

void TCPConnection::ssl_handshake() {

	/*
	* Perform a blocking handshake to stay consistent
	*/
	auto self(shared_from_this());
	std::error_code ec;
	_socket.handshake(SIDE == Side::SERVER ? asio::ssl::stream_base::server :
		asio::ssl::stream_base::client, ec);

	if (!ec) {
		open = true;
		
		read_header();

		/*
		* Prepare pong timeout checker
		*/
		send(Packet::Ping{});
		pong_timer.expires_after(std::chrono::seconds(10));
		pong_timer.async_wait(std::bind(&TCPConnection::check_pong, this));

	}
	else {
		LOG_DEBUG("ssl handshake error: %s\n", ec.message().c_str());

		close();
	}
}

void TCPConnection::psend(Packet packet) {
	const bool was_empty = out_packets.empty();
	out_packets.push_back(std::move(packet));
	if (was_empty)
		write_header();	
}

uint16_t TCPConnection::latency() {
	return latency_ms;
}

void TCPConnection::read_header() {
	if (!open)
		return;

	auto self(shared_from_this());
	asio::async_read(_socket,
		asio::buffer(&(this->temp.type), Packet::SIZE),
		[this, self](const std::error_code &e, size_t) {
		if (!e) {
			LOG_DEBUG("read_header()\n");

			if (temp.type == Packet::Type::PING) {
				LOG_DEBUG("pong!\n");
				send(Packet::Pong{});
			} // undefined behaviour arises from unexpected pongs
			else if (temp.type == Packet::Type::PONG) {
				auto now = std::chrono::steady_clock::now();
				latency_ms = static_cast<uint16_t>(
					std::chrono::duration_cast<std::chrono::milliseconds>(now - last_ping).count());
				
				LOG_DEBUG("%ums\n", latency_ms.load());

				/*
				* Send another ping after 5 seconds
				*/
				ping_timer.expires_after(std::chrono::seconds(5));
				ping_timer.async_wait(std::bind(&TCPConnection::check_ping, this));
			}

			read_body();

		}
		else {
			LOG_DEBUG("read header error: %s\n", e.message().c_str());
			close();
		}
	}
	);
}

void TCPConnection::read_body() {
	if (!open)
		return;
	
	uint16_t len = 0;
	Packet::ErrorCode p_e = Packet::S(temp.type, len);

	if (p_e != Packet::ErrorCode::OK) {
		LOG_DEBUG("read invalid packet: %u\n", (uint16_t)temp.type);
	} else if (len > 0) {
		temp.data.resize(len);

		auto self(shared_from_this());
		asio::async_read(_socket,
			asio::buffer(temp.data),
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				if (SIDE == Side::SERVER)
					this->in_packets_s->push_back({ self, temp });
				else
					this->in_packets_c->push_back(temp);

				LOG_DEBUG("read_body()\n");

				read_header();
			}
			else {

				LOG_DEBUG("read body error: %s\n", e.message().c_str());
				close();
			}
		}
		);
	}
	else {
		LOG_DEBUG("r1\n");
		read_header();
	}
}

void TCPConnection::write_header() {
	if (!open)
		return;

	/*
	* When about to send out the PING packet, record the current time
	*/ 
	if (out_packets.front().type == Packet::Type::PING) {
		last_ping = std::chrono::steady_clock::now();
	}

	auto self(shared_from_this());
	asio::async_write(_socket,
		asio::buffer(&(out_packets.front().type), Packet::SIZE),

		[this, self](const std::error_code& e, size_t) {
		if (!e) {
			LOG_DEBUG("write_header()\n");

			write_body();
		}
		else {
			LOG_DEBUG("write header error: %s\n", e.message().c_str());
			close();
		}
	}
	);
}

void TCPConnection::write_body() {
	if (!open)
		return;

	uint16_t len = Packet::sizes[(uint16_t)out_packets.front().type];
	
	if (len > 0) {
		auto self(shared_from_this());
		asio::async_write(_socket,
			asio::buffer(out_packets.front().data),
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				LOG_DEBUG("write_body()\n");

				out_packets.pop_front();

				if (!out_packets.empty())
					write_header();
			}
			else {
				LOG_DEBUG("write body error: %s\n", e.message().c_str());
				close();
			}
		}
		);
	}
	else {
		out_packets.pop_front();
		write_header();
	}
}

void TCPConnection::check_pong() {
	if (!open)
		return;

	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (pong_timer.expiry() <= asio::steady_timer::clock_type::now())
	{
		LOG_DEBUG("pong timeout\n");

		// The deadline has passed. The socket is closed so that any outstanding
		// asynchronous operations are cancelled.
		_socket.lowest_layer().close();

		// There is no longer an active deadline. The expiry is set to the
		// maximum time point so that the actor takes no action until a new
		// deadline is set.
		pong_timer.expires_at(asio::steady_timer::time_point::max());
	}

	// Put the actor back to sleep.
	pong_timer.async_wait(std::bind(&TCPConnection::check_pong, this));

}

void TCPConnection::check_ping() {
	if (!open)
		return;

	if (ping_timer.expiry() <= asio::steady_timer::clock_type::now()) {
		send(Packet::Ping{});
		pong_timer.expires_after(std::chrono::seconds(10));
	}
}
