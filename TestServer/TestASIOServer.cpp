#include <asio.hpp>
#include <iostream>

#include "tcp_server.h"

int main() {

	try {
		asio::io_context io_context;
		TCPServer server(io_context);

		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cout << "error: " << e.what() << "\n";
	}

	return 0;
}