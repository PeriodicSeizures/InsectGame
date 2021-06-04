#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <asio.hpp>
#include "network/tcp_connection.h"

using namespace asio::ip;

class TCPServer
{
public:
	// Order of these is apparently important 
	std::unordered_map<UUID, 
		std::shared_ptr<TCPConnection>> connections;

	static std::thread run_thread;

	static asio::io_context _io_context;
	tcp::acceptor _acceptor;

public:
	TCPServer(unsigned short port);
	~TCPServer();
	//void send_packet(std::shared_ptr<TCPConnection> client, Packet::Type type, void* data);
	//void process_packet(TCPConnection::pointer client, Packet packet);

	void start();

	void tick();

	/*
	* to send an assembled struct (not 'Packet' struct)
	*/
	template<class T>
	void dispatch(T packet) {
		for (auto&& conn : connections) {
			conn.second->dispatch(std::move(packet));
		}
	}

	template<class T>
	void dispatch(T packet, UUID uuid) {
		connections[uuid]->dispatch(std::move(packet));
	}

	template<class T>
	void dispatch_except(T packet, UUID uuid) {
		for (auto&& conn : connections) {
			if (conn.first != uuid)
				conn.second->dispatch(std::move(packet));
		}
	}

	void forward(Packet packet);
	void forward(Packet, UUID uuid);
	void forward_except(Packet packet, UUID uuid);

private:
	void do_accept();

};

#endif
