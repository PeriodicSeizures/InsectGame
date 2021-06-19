#ifndef SERVER_H
#define SERVER_H

#include <unordered_set>

//#pragma comment(lib, "crypt32.lib")
#include "tcp_server.h"
#include "entity/IEntity.h"
#include "PlayerList.h"

class Server final : public TCPServer
{
private:
	// special player container
	//PlayerList playerList;

	// all entities
	std::unordered_map<UUID, IEntity::ptr> uuid_entity_map;

public:
	Server(unsigned short port);
	~Server();

public:
	UUID generateUUID();

private:
	/*
	* Overridden methods
	*/
	void on_tick() override;

	/*
	* events
	*/
	bool on_join(TCPConnection::ptr) override;
	void on_packet(TCPConnection::ptr, Packet) override;
	void on_quit(TCPConnection::ptr) override;
};

extern Server* SERVER;

#endif
