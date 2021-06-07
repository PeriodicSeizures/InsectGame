#include <iostream>
#include "tcp_connection.h"

//TCPConnection::pointer TCPConnection::create(asio::io_context& _io_context) {
//	return pointer(new TCPConnection(_io_context));
//}

// used by client
TCPConnection::TCPConnection(asio::io_context& _io_context)
	: _socket(_io_context) {}

// used by server
TCPConnection::TCPConnection(tcp::socket socket)
	: _socket(std::move(socket)) {}

TCPConnection::~TCPConnection() {
	std::cout << "deconstructor()\n";
	if (_socket.is_open())
		_socket.close();
}

tcp::socket& TCPConnection::socket() {
	return _socket;
}

void TCPConnection::start() {
	read_header();
	//auto self(shared_from_this());
	//asio::post([this, self]() {
	//	write_header(); 
	//});
}

bool TCPConnection::is_connected() {
	return _socket.is_open();
}

void TCPConnection::connect_to_server(asio::io_context& _io_context, std::string host, std::string port) {

	std::cout << "attempting to connect to host: " << host << ", port: " << port << "\n";

	tcp::resolver resolver(_io_context);
	auto endpoints = resolver.resolve(tcp::v4(), host, port);

	asio::async_connect(_socket, endpoints,
		std::bind(&TCPConnection::start, this));
}

void TCPConnection::forward(Packet packet) {
	bool was_empty = out_packets.empty();
	out_packets.push_back(std::move(packet));
	if (was_empty)
		write_header();
}

/*
* if count is greater than 1, return true
* else if count is one and done writing return true
* return false
*/
bool TCPConnection::is_read_ready() {
	auto count = in_packets.count();
	if (count > 1)
		return true;
	if (count == 1 && done_reading)
		return true;
	return false;
	//return ready && in_packets.count();
}

//void TCPConnection::dispatch(Packet packet) {
//	out_packets.push_back(std::move(packet));
//}

void TCPConnection::read_header() {
	in_packets.push_back({ Packet::Type::count, nullptr }); // push an empty packet for preparation

	std::cout << "read_header()\n";

	auto self(shared_from_this());
	asio::async_read(_socket,
		asio::buffer(&(this->in_packets.back().type), Packet::SIZE),
		[this, self](const std::error_code &e, size_t) {
		if (!e) {
			done_reading = false;
			std::cout << "incoming header\n";
			// if the packet is a dummy, ignore it
			//if (in_packet_type != Packet::Type::DUMMY)
				read_body();
			//else
			//	read_header();
		}
		else {
			std::cout << "read header error: " << e.message() << "\n";
			//out_packets.clear();
			out_packets.notify();
			_socket.close();
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
		//in_packets.push_back({ in_packet_type, new char[len] });
		in_packets.back().data = new char[len];

		std::cout << "r1, " << len << "\n";

		auto self(shared_from_this());
		asio::async_read(_socket,
			asio::buffer(in_packets.back().data, len),
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				done_reading = true;
				std::cout << "incoming body\n"; // << (uint16_t)in_packet_type << "\n";
				read_header();
			}
			else {
				std::cout << "read body error: " << e.message() << "\n";
				//out_packets.clear();
				out_packets.notify();
			}
		}
		);
	}
	else {
		// go back to header
		//in_packets.push_back({ in_packet_type, nullptr });
		std::cout << "r1\n";
		read_header();
	}
}

void TCPConnection::write_header() {

	/*
	* Behaviour of AsyncQueue::wait():
	*  - present data will end
	*/
	//bool was_notified = out_packets.wait();
	//if (was_notified)
	//	return;

	//if (out_packets.count() <= 1)
	//	return;

	std::cout << "write_header()\n";

	auto self(shared_from_this());
	asio::async_write(_socket,
		asio::buffer(&(out_packets.front().type), Packet::SIZE),

		[this, self](const std::error_code& e, size_t) {
		if (!e) {
			//if (out_packets.front().type != Packet::Type::DUMMY)
				write_body();
			//else
			//	write_header();
		}
		else {
			std::cout << "write header error: " << e.message() << "\n";
		}
	}
	);
}

void TCPConnection::write_body() {
	// if the packet should contain data to write, then write

	//uint16_t len = 0;
	//Packet::ErrorCode p_e = Packet::S(out_packets.front().type, len);

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

//void TCPConnection::send_packet(Packet packet) {
//	out_packets.push_back(packet);
//}