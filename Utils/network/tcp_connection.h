#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <memory>
#include <deque>
#include <unordered_map>
#include "Packet.h"
#include "../AsyncQueue.h"
//#include "../entity/IEntity.h" //

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

//num class QuitStatus {
//	UNEXPECTED, 
//
//;

typedef asio::ssl::stream<tcp::socket> ssl_socket;
struct OwnedPacket;

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public:
	typedef std::shared_ptr<TCPConnection> ptr;

private:
	/*
	* REQUIRED CONNECTION MEMBERS
	* do not touch!
	*/
	static Side SIDE;
	static AsyncQueue<OwnedPacket> *in_packets_s;
	static AsyncQueue<Packet> *in_packets_c;
	//static std::function<TCPConnection::ptr> on_quit_handler;

	ssl_socket _socket;
	AsyncQueue<Packet> out_packets;
	Packet temp;
	bool open = false;

	//long long period_ms;
	//std::atomic_llong latency_ms;
	//bool waiting_pong;
	//std::chrono::steady_clock::time_point last_ping;

	asio::steady_timer ping_timer;
	asio::steady_timer pong_timer;
	std::atomic_llong latency_ms;
	std::chrono::steady_clock::time_point last_ping;

	//uint16_t in_rate; // count avg packets per second

public:
	/*
	* mess with these
	* 
	* mainly used by server 
	*/

	UUID uuid = -1;
	//std::string host;
	//uint16_t port;
	//EntityPlayer::ptr entity;

public:
	static void init(AsyncQueue<OwnedPacket>* in_packets_s,
		AsyncQueue<Packet>* in_packets_c);

	TCPConnection(asio::io_context &ctx, ssl_socket); // server
	~TCPConnection();

	ssl_socket& socket();

	bool is_open();
	void close();

	//UUID getUUID();
	//void setUUID(UUID uuid);

	/*
	* Begin async readers and writers
	*/
	void ssl_handshake();

	/*
	* Send data along connection
	*/
	void psend(Packet packet);

	template<class T>
	void send(T structure) {
		psend(std::move(Packet::serialize(structure)));
	}

	long long latency();

private:
	//void pinger();

	void read_header();
	void read_body();

	void write_header();
	void write_body();

	/*
	* check_pong(): Will cause a timeout for any outstanding pong
	*/
	void check_pong();
	
	/*
	* check_ping(): 
	*/
	void check_ping();

};

struct OwnedPacket {
	TCPConnection::ptr owner;
	Packet packet;
};

#endif
