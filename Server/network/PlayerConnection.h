#pragma once
#include "entity/IEntity.h"
#include "network/tcp_connection.h"
class PlayerConnection
{
private:
	UUID uuid;
	std::shared_ptr<TCPConnection> connection;
	std::string host;
	uint16_t port;
	EntityPlayer *entity;

public:
	PlayerConnection(UUID uuid,
		std::shared_ptr<TCPConnection> connection,
		EntityPlayer *entity);

};

