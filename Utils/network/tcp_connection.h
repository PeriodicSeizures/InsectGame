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
#include <asio/ssl.hpp>

using namespace asio::ip;

typedef asio::ssl::stream<tcp::socket> ssl_socket;

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
private:
	ssl_socket _socket;
	AsyncQueue<Packet> out_packets;
	Packet temp;

public:
	AsyncQueue<Packet> in_packets;

public:
	TCPConnection(ssl_socket socket);
	~TCPConnection();

	/*
	* Begin async readers and writers
	*/
	void handshake();

	/*
	* To connect to server (used by client)
	* Might cause issues if used on a server architecture
	*/
	void connect_to_server(asio::io_context& _io_context,
		std::string host,
		std::string port);


	template<class T>
	void dispatch(T packet) {
		auto size = sizeof(T);
		if (size) {
			Packet __packet = { packet.TYPE, new char[size] };
			std::memcpy(__packet.data, (void*)&packet, size);
			forward(std::move(__packet));
		}
		else {
			forward({ packet.TYPE });
		}
	}

	void forward(Packet packet);

private:


	void read_header();
	void read_body();

	void write_header();
	void write_body();

};

#endif
