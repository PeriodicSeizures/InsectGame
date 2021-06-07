#include <iostream>
#include "tcp_connection.h"

TCPConnection::TCPConnection(ssl_socket socket) 
	: _socket(std::move(socket)) {}

TCPConnection::~TCPConnection() {
	std::cout << "deconstructor()\n";
}

void TCPConnection::handshake() {

	// perform ssl handshake
	auto self(shared_from_this());
	_socket.async_handshake(asio::ssl::stream_base::server,
		[this, self](const std::error_code& e)
	{
		if (!e) {
			read_header();
		}
		else {
			std::cout << "ssl handshake error: " << e.message() << "\n";
		}
	});
}

void TCPConnection::connect_to_server(asio::io_context& _io_context, std::string host, std::string port) {
	std::cout << "attempting to connect to host: " << host << ", port: " << port << "\n";

	tcp::resolver resolver(_io_context);
	auto endpoints = resolver.resolve(tcp::v4(), host, port);

	asio::async_connect(_socket.lowest_layer(), endpoints,
		std::bind(&TCPConnection::handshake, this));
}

void TCPConnection::forward(Packet packet) {
	bool was_empty = out_packets.empty();
	out_packets.push_back(std::move(packet));
	if (was_empty)
		write_header();
}

void TCPConnection::read_header() {
	std::cout << "read_header()\n";

	auto self(shared_from_this());
	asio::async_read(_socket,
		asio::buffer(&(this->temp.type), Packet::SIZE),
		[this, self](const std::error_code &e, size_t) {
		if (!e) {
			std::cout << "incoming header\n";
			read_body();
		}
		else {
			std::cout << "read header error: " << e.message() << "\n";
		}
	}
	);
}

void TCPConnection::read_body() {
	/*
	* If packet has data to be read, then read
	*/
	
	uint16_t len = 0;
	Packet::ErrorCode p_e = Packet::S(in_packets.back().type, len);

	if (p_e != Packet::ErrorCode::OK) {
		std::cout << "invalid packet\n";
	} else if (len > 0) {
		temp.data = new char[len];

		std::cout << "r1, " << len << "\n";

		auto self(shared_from_this());
		asio::async_read(_socket,
			asio::buffer(temp.data, len),
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				this->in_packets.push_back(temp);

				std::cout << "incoming body\n"; // << (uint16_t)in_packet_type << "\n";
				read_header();
			}
			else {
				std::cout << "read body error: " << e.message() << "\n";
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

	std::cout << "write_header()\n";

	auto self(shared_from_this());
	asio::async_write(_socket,
		asio::buffer(&(out_packets.front().type), Packet::SIZE),

		[this, self](const std::error_code& e, size_t) {
		if (!e) {
			write_body();
		}
		else {
			std::cout << "write header error: " << e.message() << "\n";
		}
	}
	);
}

void TCPConnection::write_body() {
	uint16_t len = Packet::sizes[(uint16_t)out_packets.front().type];
	
	if (len > 0) {
		std::cout << "w1, " << len << "\n";

		auto self(shared_from_this());
		asio::async_write(_socket,
			asio::buffer(out_packets.front().data, len),
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				out_packets.pop_front();
				std::cout << "sent something with a body\n";

				if (!out_packets.empty())
					write_header();
			}
			else {
				std::cout << "write body error: " << e.message() << "\n";
			}
		}
		);
	}
	else {
		std::cout << "w1\n";
		out_packets.pop_front();
		write_header();
	}
}