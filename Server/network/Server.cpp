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
	
}

//static std::thread thr;

bool Server::on_join(TCPConnection::ptr connection) {

	const UUID uuid = generateUUID();
	connection->uuid = uuid;

	std::cout << uuid << " has joined\n";

	/*
	* Send uuid to client
	*/
	send_to(Packet::PlayerIdentity{ uuid }, connection);

	/* 
	* Send all players to client
	*/
	for (auto&& pair : uuid_entity_map) {
		// send the all entities declaration to client
		psend_to(pair.second->packet_new(), connection);
	}
	
	/*
	* Create its entity
	*/	
	EntityPlayer::ptr entity = std::make_shared<EntityPlayer>(uuid, "", new ServerImplPlayer());
	uuid_entity_map.insert({ uuid, entity });

	/*
	* Send player to other clients
	*/
	psend_except(entity->packet_new(), connection);

	return true;
}

void Server::on_packet(TCPConnection::ptr owner, Packet packet) {

	std::cout << "on_packet()\n";

	switch (packet.type) {
	case Packet::Type::TRANSFORM: {
		// update the entities data

		auto t = Packet::deserialize<Packet::Transform>(packet);

		//t->target = owner->uuid;
		//this->playerList.get_online_player()
		//t->target = 0;

		/*
		* dereference of packet is used, not ptr
		*/
		t->uuid = owner->uuid; // do not trust client sent uuid
		send_except(std::move(*t), owner);
		break;
	}
	//case Packet::Type::CHAT32: {
	//	auto t = Packet::deserialize<Packet::Chat32>(packet);
	//	
	//	std::cout << "chat: " << t->message << "\n";
	//}
	}
}

void Server::on_quit(TCPConnection::ptr connection) {
	std::cout << connection->uuid << " has quit\n";

	// send destroy to all players
	psend_except(uuid_entity_map[connection->uuid]->packet_delete(), connection);

	// remove entity from map
	uuid_entity_map.erase(connection->uuid);

}