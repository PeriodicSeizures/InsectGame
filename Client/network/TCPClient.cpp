#include "TCPClient.h"

TCPClient::TCPClient() 
	: _ssl_context(asio::ssl::context::tls),
	connection(
		std::make_shared<TCPConnection>(
			ssl_socket(_io_context, _ssl_context),
			&in_packets)
	) {

	//_ssl_context.load_verify_file("ca.pem");
	_ssl_context.use_private_key_file("privkey.pem", asio::ssl::context::pem);
		// just skips certificate verification since
	// its too complicated, and not upmost
	// important or necessary
	_ssl_context.set_verify_mode(asio::ssl::verify_none);

}

TCPClient::~TCPClient() {
	stop();
}

void TCPClient::start() {
	alive = true;
	ctx_thread = std::thread([this]() {
		_io_context.run();
	});

	update_thread = std::thread([this]() {
		while (alive) {
			on_update();
		}
	});

	render_thread = std::thread([this]() {
		while (alive) {
			if (do_render) {
				//std::this_thread::sleep_for(std::chrono::milliseconds(10));
				on_render();
			}
			else {
				// freeze
				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.wait(ul);
				do_render = true;
			}
		}
	});

	while (alive) {
		on_tick();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

}

void TCPClient::stop() {
	if (!alive)
		return;

	alive = false;
	_io_context.stop();

	if (ctx_thread.joinable())
		ctx_thread.join();

	in_packets.notify();

	if (update_thread.joinable())
		update_thread.join();

	if (render_thread.joinable()) {
		/*
		* if rendering is disabled, resume the cv blocking to stop block
		*/
		cvBlocking.notify_one();
		render_thread.join();
	}

	_io_context.restart();
}

void TCPClient::connect(std::string host, std::string port) {

	// io_context must not be running while connecting
	if (alive)
		return;

	// close it
	if (connection->is_open())
		connection->close();

	tcp::resolver resolver(this->_io_context);
	auto endpoints = resolver.resolve(tcp::v4(), host, port);
	
	asio::async_connect(connection->socket().lowest_layer(), endpoints,
		std::bind(&TCPConnection::handshake, connection));
}

void TCPClient::psend(Packet packet) {
	if (connection && connection->is_open())
		connection->psend(std::move(packet));
}

void TCPClient::set_render(bool a) {
	do_render = a;
	cvBlocking.notify_one();
}

void TCPClient::on_update() {
	// process incoming packets
	in_packets.wait();

	size_t limit = 5;
	while (!in_packets.empty() && limit--) {
		auto&& packet = in_packets.pop_front();

		on_packet(std::move(packet));
	}

}