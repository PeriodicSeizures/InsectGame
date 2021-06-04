#include <iostream>
#include "tcp_connection.h"

//static std::vector<char> recv_buf(8);
static char recv_buf[8];

static std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

TCPConnection::pointer TCPConnection::create(asio::io_context& io_context) {
	return pointer(new TCPConnection(io_context));
}

tcp::socket& TCPConnection::socket()
{
	return socket_;
}

void TCPConnection::start() {
	message_ = make_daytime_string();
	asio::async_write(socket_, asio::buffer(message_),
		std::bind(&TCPConnection::handle_write, shared_from_this(),
			std::placeholders::_1 // placeholder for error
			));

	asio::async_read(socket_, 
		asio::buffer(recv_buf, HEADER_SIZE),
		std::bind(&TCPConnection::handle_read_header, shared_from_this(),
			std::placeholders::_1 // placeholder for error
			));

}

TCPConnection::TCPConnection(asio::io_context& io_context)
	: socket_(io_context)//, std::enable_shared_from_this<tcp_connection>() // TEST SUPER
{
}

void TCPConnection::handle_write(const asio::error_code& e)
{
	// optional something to do after data is sent
	// ...
}

void TCPConnection::handle_read_header(const asio::error_code& e)
{
	// optional something to do after data is received
	// ...

	if (!e) {
		//if (!recv_message_.empty())
		std::cout << "incoming header: " << recv_buf << "\n";

		asio::async_read(socket_,
			asio::buffer(recv_buf, HEADER_SIZE),
			std::bind(&TCPConnection::handle_read_body, shared_from_this(),
				std::placeholders::_1 // placeholder for error
			));
	}
	else {
		// this should automatically kill the async read process,
		// but need to somehow kill the connection
		std::cout << "header error: " << e.message() << "\n";
		delete this; // kills connection
	}
}

void TCPConnection::handle_read_body(const asio::error_code& e) {
	if (!e) {
		//if (!recv_message_.empty())
		std::cout << "incoming body: " << recv_buf << "\n";

		asio::async_read(socket_,
			asio::buffer(recv_buf, HEADER_SIZE),
			std::bind(&TCPConnection::handle_read_header, shared_from_this(),
				std::placeholders::_1 // placeholder for error
			));
	}
	else {
		std::cout << "body error: " << e.message() << "\n";
		delete this;
	}
}