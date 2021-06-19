#include <limits>
#include <time.h>
#include "Server.h"

Server* SERVER;

Server::Server(unsigned short port) 
	: TCPServer(port) {}

Server::~Server() {
	
}

UUID Server::generateUUID() {
	/*
	* look through map
	*/
	UUID uuid = 0;
	uint64_t runs = 0; // shift by 1 per iteration
	while (uuid_entity_map.find(uuid) != 
		uuid_entity_map.end() && runs++ < 100) 
	{
		// keep generating a new uuid until it is unique
		// uuid |= (at << 1);
		// 32767 max
		// map 0-32,767 across 0-18,446,744,073,709,551,616

		// val of rand(), mul by 562,949,953,421,312

		srand(time(NULL));

		// uuid should now be mapped across entire 2^64 range
		uuid = rand() * 
			(std::numeric_limits<uint64_t>::max() / (1 << 15));
	
		//runs++;
	}

	return uuid;
}

void Server::on_tick() {
	//for (auto player : players) {
	//	player.second.on_tick();
	//}
	//std::cout << "tick()\n";
	
}

static std::thread thr;

bool Server::on_join(TCPConnection::ptr connection) {

	std::cout << "a player has joined\n";

	/* 
	* send all entities to 
	*/

	thr = std::thread([this, connection]() {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		Packet::Chat32 chat32 = {  };
		send_to(chat32, connection);
	});

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
	std::cout << "a player quit\n";
}