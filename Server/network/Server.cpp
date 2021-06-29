#include <limits>
#include <time.h>
#include "Server.h"
#include "../impl/ServerImpl.h"

Server* SERVER;

Server::Server(unsigned short port) 
	: TCPServer(port) {}

Server::~Server() {

}

UUID Server::generateUUID() {
	/*
	* look through map
	*/
	while (true) {
		srand(time(NULL));

		UUID uuid = rand() *
			(std::numeric_limits<uint64_t>::max() / (1 << 15));

		if (uuid_entity_map.find(uuid) ==
			uuid_entity_map.end())
			return uuid;
	}
	return 0;
}

void Server::on_tick() {
	//for (auto player : players) {
	//	player.second.on_tick();
	//}
	//std::cout << "tick()\n";
	for (auto&& entity : uuid_entity_map) {
		// call impl->on_render for all entities
		static_cast<ServerImpl*>(entity.second->impl)->behaviour(entity.second);
	}
}

//static std::thread thr;

bool Server::on_join(TCPConnection::ptr connection) {

	const UUID uuid = generateUUID();
	connection->uuid = uuid;

	std::cout << uuid << " has joined\n";

	return true;
}

void Server::on_packet(TCPConnection::ptr owner, Packet packet) {

}

void Server::on_quit(TCPConnection::ptr connection) {
	std::cout << connection->uuid << " has quit\n";

	EntityPlayer::ptr entity = std::make_shared<EntityPlayer>(connection->uuid, "", new ServerImplPlayer());
	psend_all(entity->packet_delete(), connection);

	// send destroy to all players
	//psend_except(uuid_entity_map[connection->uuid]->packet_delete(), connection);

	// remove entity from map
	//uuid_entity_map.erase(connection->uuid);

}