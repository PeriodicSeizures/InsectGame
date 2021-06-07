#include "tcp_server.h"

std::thread TCPServer::run_thread;
asio::io_context TCPServer::_io_context;

TCPServer::TCPServer(unsigned short port) : 
	_acceptor(_io_context, tcp::endpoint(tcp::v4(), port))
{
	do_accept();

	//TCPServer::_io_context.run();
}

TCPServer::~TCPServer() {
	_io_context.stop();
}

void TCPServer::start() {
	TCPServer::run_thread = std::thread(
		[]() 
		{
			_io_context.run();
			//TCPServer::_io_context.poll();
			//std::cout << "exited run\n";
			
			std::cout << "exited run\n";
		}
	);
}

void TCPServer::forward(Packet packet) {
	for (auto&& conn : connections) {
		conn.second.first->forward(std::move(packet));
	}
}

void TCPServer::forward(Packet packet, UUID uuid) {
	connections[uuid].first->forward(std::move(packet));
}

void TCPServer::forward_except(Packet packet, UUID uuid) {
	for (auto&& conn : connections) {
		if (conn.first != uuid) {
			//conn.second->out_packets.push_back(std::move(packet));
			conn.second.first->forward(std::move(packet));
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

				

				auto conn = std::make_shared<TCPConnection>(std::move(socket), uuid);


				// when here it works, not sure tho ...
				// port = ...

				std::cout << "client " << uuid << " has connected from: " << 
					addr << ", port: " << port << "\n";

				connections.insert({ uuid, 
					{conn, {Entity::Type::NONE, uuid} } 
					});

				conn->start();
			}
			catch (const std::system_error& ec) {
				std::cout << "error in connect: " << ec.what() << "\n";
			}

			//asio::steady_timer timer(_io_context);
			//timer.expires_after(std::chrono::seconds(5));
			//timer.async_wait([conn](const asio::error_code&) {
			//	Packet::Chat32 chat32 = {"Hello, World!"};
			//	conn->send_packet(Packet::serialize(chat32, chat32.type));
			//	});

			//std::make_shared<TCPConnection>(_io_context, std::move(socket))->start();
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

