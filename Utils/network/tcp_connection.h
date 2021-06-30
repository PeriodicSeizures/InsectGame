#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
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
public:
	typedef std::shared_ptr<TCPConnection> ptr;

private:
	/*
	* SHARED CONNECTION VARIABLES
	*/
	static Side SIDE;
	static AsyncQueue<OwnedPacket> *in_packets_s;
	static AsyncQueue<Packet> *in_packets_c;

	/*
	* PER CONNECTION MEMBERS
	*/
	ssl_socket _socket;
	AsyncQueue<Packet> out_packets;
	Packet temp;
	bool open = false;

	asio::steady_timer ping_timer;
	asio::steady_timer pong_timer;
	std::atomic<uint16_t> latency_ms;
	std::chrono::steady_clock::time_point last_ping;

public:
	/*
	* unused at times
	*/

	UUID uuid = -1;
	//std::string host;
	//uint16_t port;
	//EntityPlayer::ptr entity;

public:
	static void init(AsyncQueue<OwnedPacket>* in_packets_s,
		AsyncQueue<Packet>* in_packets_c);

	TCPConnection(asio::io_context &ctx, ssl_socket);
	~TCPConnection();

	ssl_socket& socket();

	bool is_open();
	void close();

	/*
	* ssl_handshake(): Performs ssl handshake, then starts reader
	*/
	void ssl_handshake();

	/*
	* [p]send(...): Send structure or data along connection
	*/
	void psend(Packet packet);

	template<class T>
	void send(T structure) {
		psend(std::move(Packet::serialize(structure)));
	}

	/*
	* latency(): Returns the latency in ms
	*/
	uint16_t latency();

private:
	void read_header();
	void read_body();

	void write_header();
	void write_body();

	void check_pong();	
	void check_ping();

};

struct OwnedPacket {
	TCPConnection::ptr owner;
	Packet packet;
};

#endif
