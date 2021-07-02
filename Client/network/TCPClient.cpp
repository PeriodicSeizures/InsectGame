#include "TCPClient.h"

TCPClient::TCPClient() 
	: _ssl_context(asio::ssl::context::tls)
{

	connection = std::make_shared<TCPConnection>(
		_io_context,
		ssl_socket(_io_context, _ssl_context));

	_ssl_context.use_private_key_file("privkey.pem", asio::ssl::context::pem);

	_ssl_context.set_verify_mode(asio::ssl::verify_none);

	TCPConnection::init(nullptr, &in_packets);

	alive = false;
	do_render = true;
}

TCPClient::~TCPClient() {
	stop();
}

void TCPClient::start() {

	if (alive)
		return;

	alive = true;
	ctx_thread = std::thread([this]() {
		_io_context.run();
	});

	//alive_ticker_thread = std::thread([this]() {
	//	while (alive) {
	//		cv.notify_one();
	//		// 50ms = 20tps
	//		// 16.6 ms = 60tps
	//		precise_sleep(1.0 / 60.0);
	//		//std::this_thread::sleep_for(std::chrono::microseconds(1666));
	//	}
	//});

	auto last_tick = std::chrono::steady_clock::now();
	auto last_render = std::chrono::steady_clock::now();

	while (alive) {
		//std::unique_lock<std::mutex> ul(mux);
		//cv.wait(ul);

		const auto now = std::chrono::steady_clock::now();

		/*
		* Tick every 16.6ms, strictly
		*/
		auto ticks_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_tick).count();
		
		if (ticks_elapsed >= 16666) { // 50000
			// tick
			// input the seconds elapsed since the last tick for delta
			on_tick(((float)ticks_elapsed) / 1000000.f);

			/*
			* Always attempt to process packets every tick
			* condition variable should be activated when packet received
			* but is not crucial
			*/
			size_t limit = 5;
			while (!in_packets.empty() && limit--) {
				auto&& packet = in_packets.pop_front();

				//on_packet(std::move(packet));

				assert(listener, "packet listener hasnt been assigned");

				listener(packet);
			}

			last_tick = std::chrono::steady_clock::now();
		}



		/*
		* Render every 16.6ms, strictly
		*   the problem with meshing render and ticker in same thread
		*   is that renderer can delay the most, and steal cycles
		*/
		auto renders_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_render).count();
		if (renders_elapsed >= 16666 && do_render) {
			// render and reset
			on_render();
			last_render = std::chrono::steady_clock::now();
		}

		/*
		* sleep time can be increased or decreased,
		* at the cost of tick and rendering accuracy and optimal speed
		* ticker will be most unaffected, due to deltaT
		*/
		precise_sleep(1.0 / 60.0);

	}

}

void TCPClient::stop() {
	if (!alive)
		return;

	alive = false;
	do_render = false;
	_io_context.stop();

	if (ctx_thread.joinable())
		ctx_thread.join();

	//cv.notify_one();
	//
	//if (alive_ticker_thread.joinable()) {
	//	/*
	//	* if rendering is disabled, resume the cv blocking to stop block
	//	*/
	//	alive_ticker_thread.join();
	//}

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
		std::bind(&TCPConnection::ssl_handshake, connection));
}

void TCPClient::psend(Packet packet) {
	if (connection && connection->is_open())
		connection->psend(std::move(packet));
}

void TCPClient::set_render(bool a) {
	if (alive)
		do_render = a;
}

uint16_t TCPClient::latency() {
	return connection->latency();
}

void TCPClient::register_listener(std::function<void(Packet)> f) {
	listener = f;
}