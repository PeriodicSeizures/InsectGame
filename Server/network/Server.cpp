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

static int tps = 0;
static auto last = std::chrono::steady_clock::now();

void Server::on_tick() {
	/*
	* Just dont do anything yet
	*/

	// count ticks per second

	auto now = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - last).count();
	if (diff > 1000000) {
		std::cout << "tps: " << tps << "/20\n";
		tps = 0;
		last = std::chrono::steady_clock::now();
	}
	tps++;



	for (auto&& entity : uuid_entity_map) {
		static_cast<ServerImpl*>(entity.second->impl)->behaviour(entity.second);
	}
}

bool Server::on_join(TCPConnection::ptr connection) {

	const UUID uuid = generateUUID();
	connection->uuid = uuid;

	std::cout << uuid << " has joined\n";

	/*
	* Send uuid to client
	*/
	send_to(Packet::S2CClientIdentity{ uuid }, connection);

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
	psend_all(entity->packet_new(), connection);

	return true;
}

void Server::on_packet(TCPConnection::ptr owner, Packet packet) {

	LOG_DEBUG("on_packet()\n");

	switch (packet.type) {
	case Packet::Type::C2S_CLIENT_INPUT: {
		// Since server is authoritive, manually manage input
		auto t = Packet::deserialize<Packet::C2SClientInput>(packet);

		// perform calculation
		EntityPlayer::ptr p =
			std::static_pointer_cast<EntityPlayer>(
				uuid_entity_map[owner->uuid]);

		// client -> send input to server

		// server -> calculate motions from input

		// send new motions back to all clients

		p->input_move(t->input_mask);
		//p->on_physics();

		send_all(Packet::S2CClientMotion{
			t->seq,
			p->x, p->y, 
			p->vx, p->vy, p->ax, p->ay, p->angle });

		// send sequenced motion back to client
		//send_to(Packet::S2CClientMotion{
		//	t->seq, p->x, p->y, p->vx, p->vy, p->ax, p->ay, p->angle},
		//	owner);
		//
		//// send motion to all other clients
		//send_all(Packet::S2CEntityMotion{ 
		//	p->uuid, p->x, p->y, p->vx, p->vy, p->ax, p->ay, p->angle },
		//	owner);

		break;
	}
	}
}

void Server::on_quit(TCPConnection::ptr connection) {
	std::cout << connection->uuid << " has quit\n";

	// send deletion packet to all clients except the quitter
	psend_all(uuid_entity_map[connection->uuid]->packet_delete(), 
		connection);

	// delete
	this->uuid_entity_map.erase(connection->uuid);
}