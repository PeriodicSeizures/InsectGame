#pragma once

#include "network/tcp_connection.h"
#include "entity/Entity.h"

struct PlayerClient {
	// a tcp connection
	//std::shared_ptr<TCPConnection> conn;

	// the current insect type
	Entity::Type insect_type;

	UUID uuid;
};