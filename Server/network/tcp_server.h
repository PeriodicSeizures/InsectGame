#ifndef TCP_SERVER_H
#define TCP_SERVER_H

// https://stackoverflow.com/questions/11821723/keeping-code-in-a-client-server-game-organized

#include <unordered_set>
#include "network/tcp_connection.h"
#include "PlayerList.h"

using namespace asio::ip;

class TCPServer
{
private:
	std::unordered_set<TCPConnection::ptr> connections;

	//PlayerList playerList;
	//std::unordered_map<UUID, IEntity::ptr> uuid_entity_map;
	//std::unordered_map<UUID, std::shared_ptr<TCPConnection>> uuid_conn_map;

	std::thread ctx_thread;
	std::thread update_thread;

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
	void psend_to(Packet packet, TCPConnection::ptr connection);

	template<class T>
	void send_to(T structure, TCPConnection::ptr connection) {
		psend_to(std::move(Packet::serialize(structure)), connection);
	}

	//template<class T>
	//void send_to(T packet, TCPConnection::ptr connection) {
	//	std::cout << "send_to()\n";
	//	if (connection && connection->is_open()) {
	//		connection->send(std::move(packet));
	//	}
	//	else {
	//		// remove
	//		disconnect(connection, false);			
	//	}
	//}

	/*
	* send_all(...): a non blocking call to send a packet along
	* all open connections
	*/
	void psend_all(Packet packet);

	template<class T>
	void send_all(T structure) {
		psend_all(std::move(Packet::serialize(structure)));
	}

	//template<class T>
	//void send_all(T packet) {
	//	for (auto&& conn : connections) {
	//		send_to(std::move(packet), conn);
	//	}
	//}

	/*
	* send_except(...): a non blocking call to send a packet along
	* all open connections except for a specific single target 
	* connection
	*/
	void psend_except(Packet packet, TCPConnection::ptr connection);

	template<class T>
	void send_except(T structure, TCPConnection::ptr connection) {
		psend_except(std::move(Packet::serialize(structure)), connection);
	}

	//template<class T>
	//void send_except(T packet, TCPConnection::ptr connection) {
	//
	//	if (!connection) {
	//		disconnect(connection, false);
	//		return;
	//	}
	//
	//	for (auto&& conn : connections) {
	//		if (conn && conn != connection)
	//			send_to(std::move(packet), conn);
	//		//conn->send(std::move(packet));
	//		else
	//			disconnect(conn, false);
	//	}
	//}

	/*
	* is_alive(): whether the io_context and associated thread is
	* alive and blocking its thread
	*/
	bool is_alive();

	/*
	* generateUUID(): generate a random uuid that is unique in
	*/
	//UUID generateUUID();

	/*
	* disconnect(...): disconnect a connection, either forcefully or peacefully
	*	- forced: kick / free
	*	- !forced: free
	*/
	void disconnect(TCPConnection::ptr connection, bool forced);

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
	virtual bool on_join(TCPConnection::ptr) = 0;
	virtual void on_packet(TCPConnection::ptr, Packet) = 0;
	virtual void on_quit(TCPConnection::ptr) = 0;

};

#endif
