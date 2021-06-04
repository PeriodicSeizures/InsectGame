#include <iostream>
#include <asio.hpp>

using namespace asio::ip;

class TCPConnection;

typedef std::shared_ptr<TCPConnection> pointer;

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public:
	TCPConnection(asio::io_context& _io_context)
		: _socket(_io_context),
		timer_latency(_io_context),
		timer_outbeat(_io_context) {}

	~TCPConnection() { std::cout << "deconstructor\n"; }

	pointer create(asio::io_context& _io_context) {
		return pointer(new TCPConnection(_io_context));
	}

	tcp::socket& socket() {
		return _socket;
	}

	void start() {

		// must receive a heartbeat (inbeat) within every 30 seconds, else
		// conenction shall close

		std::cout << "start()\n";

		read_header();
	}

	bool is_connected() {
		return _socket.is_open();
	}

	void connect_to_server(asio::io_context& _io_context, std::string host, std::string port) {
		tcp::resolver resolver(_io_context);
		auto endpoints = resolver.resolve(tcp::v4(), host, port);

		// 3 second timeout for connect

		std::cout << "after deadline set\n";

		timer_latency.expires_after(std::chrono::seconds(10));

		asio::async_connect(_socket, endpoints,
			std::bind(&TCPConnection::start, this));

		timer_latency.async_wait(
			std::bind(&TCPConnection::check_inbeat, this)); // , std::placeholders::_1

	}



	void read_header() {
		/*
		* Set expiration to x seconds
		* where check_inbeat() is called to check the expiration
		*/
		timer_latency.expires_after(std::chrono::seconds(15));
		timer_latency.async_wait(
			std::bind(&TCPConnection::check_inbeat, this));

		auto self(shared_from_this());

		asio::async_read(_socket,
			asio::buffer(&in_packet_type, Packet::SIZE),
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				read_body();
			}
			else {
				std::cout << "read body error: " << e.message() << "\n";

				std::cout << "this is null: " << (this == nullptr) << "\n";

				close();

				//delete this;
			}
		}
		);
	}

	void read_body() {
		/*
		* If packet has data to be read, then read
		*/
		std::cout << "read_body()\n";
		uint16_t len = 0;
		Packet::ErrorCode p_e = Packet::S(in_packet_type, len);

		if (p_e != Packet::ErrorCode::OK) {
			std::cout << "invalid packet\n";
			//_socket.close();
			//timer_latency.cancel();
			//timer_outbeat.cancel();

			close();

			delete this;
			return;
		}

		if (len > 0) {
			in_packets.push_back({ in_packet_type, new char[len] });

			auto self(shared_from_this());

			asio::async_read(_socket,
				asio::buffer(in_packets.back().data, len),
				[this, self](const std::error_code& e, size_t) {
				if (!e) {
					std::cout << "incoming header: " << (uint16_t)in_packet_type << "\n";
					read_header();
				}
				else {
					std::cout << "read header error: " << e.message() << "\n";

					close();

					delete this;
				}
			}
			);
		}
		else {
			// go back to header
			in_packets.push_back({ in_packet_type, nullptr });
			read_header();
		}
	}

	void write_header() {
		if (!out_packets.empty()) {
			asio::async_write(_socket,
				asio::buffer(&out_packets.front().type, Packet::SIZE),
				[this](const std::error_code& e, size_t) {
				if (!e) {
					write_body();
				}
				else {
					std::cout << "write header error: " << e.message() << "\n";

					close();

					delete this;
				}
			}
			);
		}
		else {
			// write heartbeat for 5 seconds from now
			timer_outbeat.expires_after(std::chrono::seconds(5));
			timer_outbeat.async_wait(
				std::bind(&TCPConnection::write_outbeat, this));
		}
	}

	void write_body() {
		// if the packet should contain data to write, then write

		uint16_t len = 0;
		Packet::ErrorCode p_e = Packet::S(out_packets.front().type, len);

		if (p_e != Packet::ErrorCode::OK) {
			std::cout << "invalid packet\n";
			delete this;
			return;
		}

		if (len > 0) {

			assert(out_packets.front().data, "tried writing a packet assumed to have data but it has no data");

			asio::async_write(_socket,
				asio::buffer(out_packets.front().data, len),
				[this](const std::error_code& e, size_t) {
				if (!e) {
					write_header();
				}
				else {
					std::cout << "write body error: " << e.message() << "\n";

					close();

					delete this;
				}
			}
			);
		}
	}

	void check_inbeat() { // const asio::error_code& e
		// Check whether the deadline has passed. We compare the deadline against
		// the current time since a new asynchronous operation may have moved the
		// deadline before this actor had a chance to run.

		std::cout << "check_deadline()\n";

		if (timer_latency.expiry() <= asio::steady_timer::clock_type::now())
		{
			close();

			delete this;
			return;

			//std::cout << "closing\n";
			//_socket.close();

			// There is no longer an active deadline. The expiry is set to the
			// maximum time point so that the actor takes no action until a new
			// deadline is set.
			//timer_latency.expires_at(asio::steady_timer::time_point::max());
			//timer_latency.cancel();
			//return;
		}

		// Put the actor back to sleep.
		timer_latency.async_wait(
			std::bind(&TCPConnection::check_inbeat, this));
	}

	/*
	* Write a heartbeat 5 seconds after the previous packet
	*/

	void write_outbeat() {
		if (timer_outbeat.expiry() <= asio::steady_timer::clock_type::now())
		{
			// write heartbeat
		}

		// Put the actor back to sleep.
		timer_outbeat.async_wait(
			std::bind(&TCPConnection::write_outbeat, this));

	}

	void close() {
		//return;
		//if (_socket.is_open())
		//_socket.close();
		return;

		timer_latency.expires_at(asio::steady_timer::time_point::max());
		timer_outbeat.expires_at(asio::steady_timer::time_point::max());

		_socket.close();
	}

	private:
		tcp::socket _socket;
		Packet::Type in_packet_type;
		//AsyncQueue<Packet> in_packets;
		//AsyncQueue<Packet> out_packets;
		//asio::steady_timer deadline;
		asio::steady_timer timer_latency;
		asio::steady_timer timer_outbeat;

};