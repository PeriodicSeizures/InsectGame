#ifndef SERVER_H
#define SERVER_H

#include <unordered_set>

//#pragma comment(lib, "crypt32.lib")
#include "network/tcp_server.h"

class Server final : public TCPServer
{
private:
	//std::unordered_map<UUID, std::shared_ptr<TCPConnection>>
	//	mapped_players;

public:
	Server(unsigned short port);
	~Server();

private:
	/*
	* Overridden methods
	*/
	void on_tick() override;

	/*
	* events
	*/
	bool on_join(std::shared_ptr<TCPConnection>) override;
	void on_packet(std::shared_ptr<TCPConnection>, Packet) override;
	void on_quit(std::shared_ptr<TCPConnection>) override;
};

extern Server* SERVER;

#endif
