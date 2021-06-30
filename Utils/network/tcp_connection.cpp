#include <iostream>
#include "tcp_connection.h"

Side TCPConnection::SIDE;
AsyncQueue<OwnedPacket>* TCPConnection::in_packets_s;
AsyncQueue<Packet>* TCPConnection::in_packets_c;
//std::function<TCPConnection::ptr> TCPConnection::on_quit_handler;

void TCPConnection::init(AsyncQueue<OwnedPacket>* in_packets_s,
	AsyncQueue<Packet>* in_packets_c) 
{
	if (in_packets_s)
		SIDE = Side::SERVER;
	else if (in_packets_c) SIDE = Side::CLIENT;
	else throw "no packet queue has been assigned";

	TCPConnection::in_packets_s = in_packets_s;
	TCPConnection::in_packets_c = in_packets_c;
	//TCPConnection::on_quit_handler = on_quit_handler;
}

// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/example/cpp11/ssl/client.cpp
TCPConnection::TCPConnection(asio::io_context& ctx, ssl_socket socket)
	: _socket(std::move(socket)),
	ping_timer(ctx),
	pong_timer(ctx)
{
	/*
	* clientside certificate init
	*/

	_socket.set_verify_mode(asio::ssl::verify_none);

	//_socket.set_verify_mode(asio::ssl::verify_peer);
	//_socket.set_verify_callback(
	//	std::bind(&verify_certificate, std::placeholders::_1, std::placeholders::_2));

}

TCPConnection::~TCPConnection() {
	std::cout << "deconstructor()\n";
}

ssl_socket& TCPConnection::socket() {
	return _socket;
}

bool TCPConnection::is_open() {
	// return the status
	//return _socket.lowest_layer().is_open();
	return open;
}

void TCPConnection::close() {
	open = false;

	asio::error_code ec;
	_socket.lowest_layer().close(ec);

	pong_timer.cancel();
}

void TCPConnection::ssl_handshake() {

	//send(Packet::Ping{});

	// perform ssl handshake
	auto self(shared_from_this());
	std::error_code ec;
	_socket.handshake(SIDE == Side::SERVER ? asio::ssl::stream_base::server :
		asio::ssl::stream_base::client, ec);

	if (!ec) {
		open = true;
		//if (SIDE == Side::CLIENT)
		
		read_header();

		send(Packet::Ping{});
		pong_timer.expires_after(std::chrono::seconds(10));
		pong_timer.async_wait(std::bind(&TCPConnection::check_pong, this));

		// Send a ping
		


		//if (SIDE == Side::CLIENT)
			//pinger();

	}
	else {
		std::cout << "ssl handshake error: " << ec.message() << "\n";
	}

	//_socket.async_handshake(SIDE==Side::SERVER ? asio::ssl::stream_base::server :
	//	asio::ssl::stream_base::client,
	//	[this, self](const std::error_code& e)
	//{
	//	if (!e) {
	//		open = true;
	//		//if (SIDE == Side::CLIENT)
	//			read_header();
	//		//do nothing yet
	//	}
	//	else {
	//		std::cout << "ssl handshake error: " << e.message() << "\n";
	//	}
	//});
}

void TCPConnection::psend(Packet packet) {
	const bool was_empty = out_packets.empty();
	out_packets.push_back(std::move(packet));
	if (was_empty)
		write_header();	
}

long long TCPConnection::latency() {
	return latency_ms;
}

//void TCPConnection::pinger() {
//	// start a tbread to ping every period_ms ms
//
//	asio::post([this]() {
//		// start pinger
//		while (open) {
//			// immediately send a ping
//			std::cout << "ping\n";
//			send(Packet::Ping{});
//			waiting_pong = true;
//
//			// if the other fails to respond in 5 seconds, destroy connection
//			std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//
//			// ping must have been received and updated by then
//			// aka if still waiting for pong
//			if (waiting_pong) {
//				// destroy connection and break
//				// this assumes that the connection has abruptly stopped responding
//				std::cout << "no pong received\n";
//				if (open)
//					_socket.lowest_layer().close();
//
//				open = false;
//
//				break;
//			}
//			else {
//				// send another pong, 
//				// aka do nothing as this loops
//			}
//		}
//	});
//}

void TCPConnection::read_header() {
	if (!open)
		return;

	// each time a new read is performed, this will reset the timer

	auto self(shared_from_this());
	asio::async_read(_socket,
		asio::buffer(&(this->temp.type), Packet::SIZE),
		[this, self](const std::error_code &e, size_t) {
		if (!e) {
			LOG_DEBUG("read_header()");

			// always respond to a ping with a 'pong'
			//if (temp.type == Packet::Type::PING) {
			//	std::cout << "pong!\n";
			//	send(Packet::Pong{});
			//	//read_header();
			//} // if an expected pong is received, measure the duration
			//else if (temp.type == Packet::Type::PONG) {
			//	if (waiting_pong) {
			//		// toggle wait
			//		waiting_pong = false;
			//		auto now = std::chrono::steady_clock::now();
			//		latency_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_ping).count();
			//		//read_header();
			//	}
			//	else {
			//
			//	}
			//	//read_header();
			//}

			if (temp.type == Packet::Type::PING) {
				std::cout << "pong!\n";
				send(Packet::Pong{});
			} // behaviour will be undefined if a pong was not expected but was received
			else if (temp.type == Packet::Type::PONG) {
				auto now = std::chrono::steady_clock::now();
				latency_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_ping).count();
				
				std::cout << latency_ms << "ms\n";

				/*
				* In 5 seconds, send another ping
				*/
				ping_timer.expires_after(std::chrono::seconds(5));
				ping_timer.async_wait(std::bind(&TCPConnection::check_ping, this));

				//ping_timer.async_wait([this]() {
				//	if (!open)
				//		return;
				//
				//	if (ping_timer.expiry() <= asio::steady_timer::clock_type::now()) {
				//		send(Packet::Ping{});
				//		pong_timer.expires_after(std::chrono::seconds(10));
				//	}
				//});

			}

			read_body();

		}
		else {
			std::cout << "read header error: " << e.message() << "\n";
			close();
			//if (SIDE == Side::SERVER) in_packets_s->notify();
		}
	}
	);
}

void TCPConnection::read_body() {
	/*
	* If packet has data to be read, then read
	*/

	if (!open)
		return;
	
	uint16_t len = 0;
	Packet::ErrorCode p_e = Packet::S(temp.type, len);

	if (p_e != Packet::ErrorCode::OK) {
		std::cout << "invalid packet\n";
	} else if (len > 0) {
		//temp.data = new char[len];
		temp.data.resize(len);

		auto self(shared_from_this());
		asio::async_read(_socket,
			asio::buffer(temp.data), // len
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				if (SIDE == Side::SERVER)
					this->in_packets_s->push_back({ self, temp });
				else
					this->in_packets_c->push_back(temp);

				LOG_DEBUG("read_body()");

				//std::cout << "incoming body\n"; // << (uint16_t)in_packet_type << "\n";
				read_header();
			}
			else {
				std::cout << "read body error: " << e.message() << "\n";
				close();
				//if (SIDE == Side::SERVER) in_packets_s->notify();
			}
		}
		);
	}
	else {
		std::cout << "r1\n";
		read_header();
	}
}

void TCPConnection::write_header() {

	if (!open)
		return;

	LOG_DEBUG("write_header()");

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
			//if (out_packets.front().type == )

			write_body();
		}
		else {
			std::cout << "write header error: " << e.message() << "\n";
			close();
			//if (SIDE == Side::SERVER) in_packets_s->notify();
		}
	}
	);
}

void TCPConnection::write_body() {
	if (!open)
		return;

	uint16_t len = Packet::sizes[(uint16_t)out_packets.front().type];
	
	if (len > 0) {
		LOG_DEBUG("write_body()");

		auto self(shared_from_this());
		asio::async_write(_socket,
			asio::buffer(out_packets.front().data), // len
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				out_packets.pop_front();

				if (!out_packets.empty())
					write_header();
			}
			else {
				std::cout << "write body error: " << e.message() << "\n";
				close();
				//if (SIDE == Side::SERVER) in_packets_s->notify();
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
		std::cout << "pong timeout\n";

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

//void TCPConnection::reset_pinger() {
//	pinger.expires_after(std::chrono::seconds(10));
//}