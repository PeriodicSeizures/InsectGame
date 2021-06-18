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

enum class Side {
	CLIENT, SERVER
};

typedef asio::ssl::stream<tcp::socket> ssl_socket;
struct OwnedPacket;

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
private:
	const Side SIDE;

	ssl_socket _socket;
	AsyncQueue<Packet> out_packets;
	Packet temp;

	AsyncQueue<OwnedPacket> *in_packets_s;
	AsyncQueue<Packet> *in_packets_c;

//public:
//	UUID uuid;

public:
	TCPConnection(ssl_socket, AsyncQueue<OwnedPacket>*); // server
	TCPConnection(ssl_socket, AsyncQueue<Packet>*); // client
	~TCPConnection();

	ssl_socket& socket();

	bool is_open();
	void close();

	//UUID getUUID();

	/*
	* Begin async readers and writers
	*/
	void handshake();

	/*
	* Send data along connection
	*/
	template<class T>
	void send(T p) {
		//auto t = p.TYPE;
		//auto t2 = Packet::Type::CHAT128;
		Packet __packet = { p.TYPE };
		//__packet.type = decltype(packet)::TYPE;
		if (sizeof(T)) {
			__packet.data.resize(sizeof(T));
			std::memcpy(__packet.data.data(), (void*)&p, sizeof(T));
		}
		bool was_empty = out_packets.empty();
		out_packets.push_back(std::move(__packet));
		if (was_empty)
			write_header();
	}

private:
	void read_header();
	void read_body();

	void write_header();
	void write_body();
};

struct OwnedPacket {
	std::shared_ptr<TCPConnection> owner;
	Packet packet;
};

#endif
