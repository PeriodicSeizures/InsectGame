#pragma once
#include <asio.hpp>
#include "tcp_connection.h"

using namespace asio::ip;

class TCPServer
{
public:
	TCPServer(asio::io_context& io_context);

private:
	void start_accept();

	void handle_accept(TCPConnection::pointer new_connection,
		const asio::error_code& error);

	tcp::acceptor acceptor_; // (io_context, tcp::endpoint(tcp::v4(), 13));
	asio::io_context& io_context_;

	//std::unordered_map<uint32_t>
};

