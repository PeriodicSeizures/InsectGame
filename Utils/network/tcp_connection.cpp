#include <iostream>
#include "tcp_connection.h"

TCPConnection::TCPConnection(ssl_socket socket, AsyncQueue<OwnedPacket>* in_packets_s)
	: _socket(std::move(socket)), 
	in_packets_s(in_packets_s), 
	in_packets_c(nullptr),
	SIDE(Side::SERVER)
{

	_socket.set_verify_mode(asio::ssl::verify_none);
}

// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/example/cpp11/ssl/client.cpp
TCPConnection::TCPConnection(ssl_socket socket, AsyncQueue<Packet>* in_packets_c)
	: _socket(std::move(socket)), 
	in_packets_s(nullptr),
	in_packets_c(in_packets_c), 
	SIDE(Side::CLIENT)
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
	_socket.lowest_layer().close();
	//std::error_code e;
	//_socket.lowest_layer().cancel();
	//_socket.async_shutdown([this]() { _socket.lowest_layer().close(); });
	//const char buf[] = "";
	//asio::async_write(_socket, asio::buffer(buf),
	//	[this]() {_socket.lowest_layer().close(); });
}

//UUID TCPConnection::getUUID() {
//	return uuid;
//}
//
//void TCPConnection::setUUID(UUID uuid) {
//	this->uuid = uuid;
//}

void TCPConnection::ssl_handshake() {

	// perform ssl handshake
	auto self(shared_from_this());

	std::error_code ec;
	_socket.handshake(SIDE == Side::SERVER ? asio::ssl::stream_base::server :
		asio::ssl::stream_base::client, ec);

	if (!ec) {
		open = true;
		//if (SIDE == Side::CLIENT)
		read_header();
		pinger();
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

void TCPConnection::pinger() {
	// start a tbread to ping every period_ms ms
	asio::post([this]() {
		// start pinger
		while (open) {
			// immediately send a ping
			send(Packet::Ping{});
			waiting_pong = true;

			// if the other fails to respond in 5 seconds, destroy connection
			std::this_thread::sleep_for(std::chrono::milliseconds(5000));

			// ping must have been received and updated by then
			// aka if still waiting for pong
			if (waiting_pong) {
				// destroy connection and break
				// this assumes that the connection has abruptly stopped responding
				close();
				break;
			}
			else {
				// send another pong, 
				// aka do nothing as this loops
			}
		}
	});
}

void TCPConnection::read_header() {
	auto self(shared_from_this());
	asio::async_read(_socket,
		asio::buffer(&(this->temp.type), Packet::SIZE),
		[this, self](const std::error_code &e, size_t) {
		if (!e) {
			LOG_DEBUG("read_header()");

			// always respond to a ping with a 'pong'
			if (temp.type == Packet::Type::PING) {
				send(Packet::Pong{});
				read_header();
			} // if an expected pong is received, measure the duration
			else if (temp.type == Packet::Type::PONG && waiting_pong) {
				// toggle wait
				waiting_pong = false;
				auto now = std::chrono::steady_clock::now();
				latency_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_ping).count();
				read_header();
			}
			else {
				read_body();
			}

				

		}
		else {
			std::cout << "read header error: " << e.message() << "\n";
			open = false;
			//if (SIDE == Side::SERVER) in_packets_s->notify();
		}
	}
	);
}

void TCPConnection::read_body() {
	/*
	* If packet has data to be read, then read
	*/
	
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
				open = false;
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

	LOG_DEBUG("write_header()");

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
			open = false;
			//if (SIDE == Side::SERVER) in_packets_s->notify();
		}
	}
	);
}

void TCPConnection::write_body() {
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
				open = false;
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

//void TCPConnection::begin_measures() {
//	// write pings
//
//	send(Packet::Ping{});
//}