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
	*/
	//_ssl_context.set_options(
	//	asio::ssl::context::default_workarounds
	//	| asio::ssl::context::no_sslv2
	//	| asio::ssl::context::single_dh_use);
	_ssl_context.set_verify_mode(asio::ssl::verify_none);
	//_ssl_context.set_password_callback(std::bind(&get_password));
	//_ssl_context.cer
	_ssl_context.use_certificate_chain_file("newcert.pem");
	_ssl_context.use_private_key_file("privkey.pem", asio::ssl::context::pem);
	//// for diffie helman?
	_ssl_context.use_tmp_dh_file("dh1024.pem");

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

	while (alive) {
		on_tick();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

void TCPServer::on_update() {

	in_packets.wait();

	size_t limit = 20;
	while (!in_packets.empty() && limit--) {
		auto&& owned_packet = in_packets.pop_front();

		// might have issue
		if (!owned_packet.owner->is_open())
			disconnect(owned_packet.owner, false);
		else
			on_packet(owned_packet.owner, std::move(owned_packet.packet));

		//owned_packet.owner.reset()
	}
}

bool TCPServer::is_alive() {
	return alive;
}

void TCPServer::disconnect(TCPConnection::ptr connection, bool forced) {

	// kick
	if (forced) {
		connection->close();
	}

	// free
	on_quit(connection);
	connections.erase(connection);
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
					ssl_socket(std::move(socket), _ssl_context),
					&in_packets);

				// Whether to accept or deny the connection
				if (on_join(conn)) {
					conn->handshake();
					connections.insert(conn);
				}
				conn.reset();
			}
			catch (const std::system_error& e) 
			{
				std::cout << "error in connect: " << e.what() << "\n";
			}
		}

		do_accept(); // loops back to continue accept
	});

}

