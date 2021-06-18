#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "network/tcp_connection.h"
class TCPClient
{
private:

	asio::io_context _io_context;
	asio::ssl::context _ssl_context;
	
	std::thread ctx_thread;
	std::thread update_thread;
	std::thread render_thread;

	AsyncQueue<Packet> in_packets;

	std::shared_ptr<TCPConnection> connection;

	std::atomic_bool alive = false;

	// for rendering
	std::atomic_bool do_render = true;
	std::condition_variable cvBlocking;
	std::mutex muxBlocking;

public:
	TCPClient();
	virtual ~TCPClient();

	/*
	* start(): a blocking call to begin the client connect to 
	* server and listen, assuming connect was successful
	*/
	void start();

	/*
	* stop(): a blocking call which terminates all active 
	* server threads (run, update, render)
	*/
	void stop();

	/*
	* connect(...): a non blocking call to connect to a target 
	* ssl encrypted server
	*/
	void connect(std::string host, std::string port);

	/*
	* send(...): a non blocking call to send packet to server
	*/
	template<class T>
	void send(T packet) {
		connection->send(std::move(packet));
	}

	/*
	* set_render(...): enable or disable rendering thread
	*/
	void set_render(bool a);

private:
	void on_update();

	virtual void on_tick() = 0;
	virtual void on_render() = 0;

	virtual void on_packet(Packet) = 0;
};

#endif