#include "tcp_server.h"

//std::thread TCPServer::run_thread;
//asio::io_context TCPServer::_io_context;

TCPServer::TCPServer(unsigned short port) :
	_acceptor(_io_context, tcp::endpoint(tcp::v4(), port)),
	_ssl_context(asio::ssl::context::tls)
{
	do_accept();

	//TCPServer::_io_context.run();
}

TCPServer::~TCPServer() {
	_io_context.stop();
}

void TCPServer::start() {
	TCPServer::run_thread = std::thread(
		[this]()
	{
		_io_context.run();

		std::cout << "exited run\n";
	}
	);
}

void TCPServer::forward(Packet packet) {
	for (auto&& conn : connections) {
		conn.second->forward(std::move(packet));
	}
}

void TCPServer::forward(Packet packet, UUID uuid) {
	connections[uuid]->forward(std::move(packet));
}

void TCPServer::forward_except(Packet packet, UUID uuid) {
	for (auto&& conn : connections) {
		if (conn.first != uuid) {
			//conn.second->out_packets.push_back(std::move(packet));
			conn.second->forward(std::move(packet));
		}
	}
}

void TCPServer::do_accept()
{
	//std::hash<std::string> hasher;
	//UUID uuid = hasher(socket_.remote_endpoint().address().to_string());

	_acceptor.async_accept(
		[this](const asio::error_code& ec, tcp::socket socket)
	{
		if (!ec)
		{
			try {
				asio::ip::tcp::endpoint endpoint = socket.remote_endpoint();

				std::string addr = endpoint.address().to_string();
				uint16_t port = endpoint.port();

				std::hash<std::string> hasher;
				UUID uuid = hasher(addr + std::to_string(port));



				auto conn = std::make_shared<TCPConnection>(
					ssl_socket(std::move(socket), _ssl_context));

				//std::cout << "client " << uuid << " has connected from: " << 
				//	addr << ", port: " << port << "\n";

				connections.insert({ uuid, conn });
				conn->handshake();
			}
			catch (const std::system_error& e) {
				std::cout << "error in connect: " << e.what() << "\n";
			}
		}

		do_accept(); // loops back to continue accept
	});


	/*
	* jsut print all incoming messages
	*/
	//while (true) {
	//	for (auto&& entry : connections) {
	//		//entry.second->
	//	}
	//}

}

