#include "tcp_server.h"

//static std::string get_password()
//{
//	return "test";
//}

TCPServer::TCPServer(unsigned short port) :
	_acceptor(_io_context, tcp::endpoint(tcp::v4(), port)),
	_ssl_context(asio::ssl::context::tls) {
	/*
	* certificate and encryption init
	* 
	* this is probably all wrong
	*/
	_ssl_context.set_verify_mode(asio::ssl::verify_none);

	_ssl_context.use_certificate_chain_file("newcert.pem");
	_ssl_context.use_private_key_file("privkey.pem", asio::ssl::context::pem);

	_ssl_context.use_tmp_dh_file("dh1024.pem");

	TCPConnection::init(&in_packets, nullptr);
}

TCPServer::~TCPServer() {
	stop();
}

void TCPServer::start() {
	do_accept();

	alive = true;
	ctx_thread = std::thread([this](){
		_io_context.run();
	});

	update_thread = std::thread([this]() {
		while (alive) {
			on_update();
		}
	});

	auto last_tick = std::chrono::steady_clock::now();
	while (alive) {

		const auto now = std::chrono::steady_clock::now();

		auto ticks_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();

		on_tick(((float)ticks_elapsed) / 1000000.f);
		last_tick = std::chrono::steady_clock::now();
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
		precise_sleep(0.05);
	}
}

void TCPServer::stop() {
	alive = false;
	_io_context.stop();

	if (ctx_thread.joinable())
		ctx_thread.join();

	if (update_thread.joinable())
		update_thread.join();

	_io_context.restart();
}

void TCPServer::psend_to(Packet packet, TCPConnection::ptr connection) {

	LOG_DEBUG("send_to()\n");
	if (connection && connection->is_open()) {
		connection->psend(std::move(packet));
	}
	else {
		// remove
		//disconnect(connection);

		// event
		on_quit(connection);

		// nullify the local instance to have to living reference
		connection.reset();

		// 'connection' still refers to the deque (in this case set)
		// remove the instance of connection from connections
		connections.erase(connection);
	}	
}

void TCPServer::psend_all(Packet packet, TCPConnection::ptr except) {

	// use std iterator to remove and increment
	for (auto it = connections.begin(); it != connections.end(); ) {

		auto conn = *it;

		if (conn && conn->is_open()) {
			// send
			if (conn != except)
				conn->psend(std::move(packet));
			++it;
		}
		else {
			it = connections.erase(it);

			// event
			on_quit(conn);
		}
	}

}

void TCPServer::on_update() {

	in_packets.wait();

	/*
	* process packets
	*/
	size_t limit = 20;
	while (!in_packets.empty() && limit--) {
		auto&& owned_packet = in_packets.pop_front();

		on_packet(owned_packet.owner, std::move(owned_packet.packet));
	}

}

bool TCPServer::is_alive() {
	return alive;
}

void TCPServer::do_accept()
{
	_acceptor.async_accept(
		[this](const asio::error_code& ec, tcp::socket socket)
	{
		if (!ec)
		{
			try 
			{
				asio::ip::tcp::endpoint endpoint = socket.remote_endpoint();

				std::string addr = endpoint.address().to_string();
				uint16_t port = endpoint.port();

				std::hash<std::string> hasher;
				UUID uuid = hasher(addr + std::to_string(port));



				auto conn = std::make_shared<TCPConnection>(
					_io_context,
					ssl_socket(std::move(socket), _ssl_context));

				// always establish connection first
				// handshake will be blocking
				conn->ssl_handshake();

				// Whether to accept or deny the connection
				if (on_join(conn)) {
					connections.insert(conn);
				}
				else {
					/*
					* something to do on deny ...
					*/
				}
				conn.reset();
			}
			catch (const std::system_error& e) 
			{
				LOG_DEBUG("connect error: %s\n", e.what());
			}
		}

		do_accept();
	});

}

