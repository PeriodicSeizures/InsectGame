#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <deque>
#include <unordered_map>
#include "Packet.h"
#include "../AsyncQueue.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE 1
#include <asio.hpp>

using namespace asio::ip;

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
private:
	tcp::socket _socket;
	//Packet::Type in_packet_type;

	AsyncQueue<Packet> out_packets;
	std::atomic_bool done_reading = false;

public:
	AsyncQueue<Packet> in_packets;

	//UUID uuid;
	//asio::steady_timer deadline;

	//asio::steady_timer timer_latency;
	//std::chrono::steady_clock::time_point last, now;
	//std::chrono::milliseconds prev_latency;
	//asio::steady_timer timer_out, timer_in;

public:
	TCPConnection(asio::io_context& _io_context);
	TCPConnection(tcp::socket socket);
	~TCPConnection();

	tcp::socket& socket();
	
	bool is_connected();

	/*
	* Begin async readers and writers
	*/
	void start();

	/*
	* To connect to server (used by client)
	* Might cause issues if used on a server architecture
	*/
	void connect_to_server(asio::io_context& _io_context, 
		std::string host, 
		std::string port);

	//void send_packet(Packet packet);

	template<class T>
	void dispatch(T packet) {
		//static_assert()
		auto size = sizeof(T); // Packet::sizes[(uint16_t)packet.type];
		if (size) {
			Packet __packet = { packet.TYPE, new char[size] };
			std::memcpy(__packet.data, (void*)&packet, size);
			forward(std::move(__packet));

			//out_packets.push_back(std::move(__packet));
		}
		else {
			//out_packets.push_back({ packet.type });
			forward({ packet.TYPE });
		}
	}

	void forward(Packet packet);

	bool is_read_ready();

	// send raw packet
	//void dispatch(Packet packet);

private:
	

	void read_header();
	void read_body();

	void write_header();
	void write_body();

	//void check_time(); // const std::error_code &e
	//void write_outbeat();

	//void close();

	// packet about 300-400 bytes (512 safe)
	// 512 bytes is on the larger side for an ordinary tcp tx/rx packet
	// zip ~ 62%

};

#endif
