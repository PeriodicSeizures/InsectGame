#ifndef TCP_SERVER_H
#define TCP_SERVER_H

// https://stackoverflow.com/questions/11821723/keeping-code-in-a-client-server-game-organized

#include <unordered_set>
#include "network/tcp_connection.h"

using namespace asio::ip;

class TCPServer
{
private:
	std::unordered_set<std::shared_ptr<TCPConnection>> connections;

	std::thread ctx_thread;
	std::thread update_thread;
	//std::thread tick_thread;

	asio::io_context _io_context;
	asio::ssl::context _ssl_context;
	tcp::acceptor _acceptor;

	AsyncQueue<OwnedPacket> in_packets;

	std::atomic_bool alive = false;

public:
	TCPServer(unsigned short port);
	virtual ~TCPServer();

	/*
	* start(): a blocking call to begin the server and start
	* accepting incoming connections and listening
	*/
	void start();

	/*
	* stop(): a blocking call which terminates the io run thread
	*/
	void stop();

	/*
	* send_to(...): a non blocking call to send a packet along a
	* target connection
	*/
	template<class T>
	void send_to(T packet, std::shared_ptr<TCPConnection> connection) {
		if (connection && connection->is_open()) {
			connection->send(std::move(packet));
		}
		else {
			// remove
			connection.reset();
			connections.erase(connection);
		}
	}

	/*
	* send_all(...): a non blocking call to send a packet along
	* all open connections
	*/
	template<class T>
	void send_all(T packet) {
		for (auto&& conn : connections) {
			send_to(std::move(packet), conn);
		}
	}

	/*
	* send_except(...): a non blocking call to send a packet along
	* all open connections except for a specific single target 
	* connection
	*/
	template<class T>
	void send_except(T packet, std::shared_ptr<TCPConnection> connection) {
		for (auto&& conn : connections) {
			if (conn != connection)
				send_to(std::move(packet), conn);
				//conn->send(std::move(packet));
		}
	}

	/*
	* is_alive(): whether the io_context and associated thread is
	* alive and blocking its thread
	*/
	bool is_alive();

private:
	/*
	* Server logic methods
	*/
	void on_update();
	virtual void on_tick() = 0;

	void do_accept();

	/*
	* Client based events / methods
	*/
	virtual bool on_join(std::shared_ptr<TCPConnection> connection) = 0;
	virtual void on_packet(std::shared_ptr<TCPConnection> owner, Packet packet) = 0;
	virtual void on_quit(std::shared_ptr<TCPConnection> connection) = 0;

};

#endif
