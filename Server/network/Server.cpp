#include "Server.h"

Server* SERVER;

Server::Server(unsigned short port) 
	: TCPServer(port) {}

Server::~Server() {
	
}

void Server::on_tick() {
	//for (auto player : players) {
	//	player.second.on_tick();
	//}
	//std::cout << "tick()\n";
	
}

bool Server::on_join(std::shared_ptr<TCPConnection> connection) {

	std::cout << "a player has joined\n";

	return true;
}

void Server::on_packet(std::shared_ptr<TCPConnection> owner, Packet packet) {

	std::cout << "on_packet()\n";

	switch (packet.type) {
	case Packet::Type::TRANSFORM: {
		// update the entities data

		auto t = Packet::deserialize<Packet::Transform>(packet);

		t->target = owner->uuid;

		/*
		* dereference of packet is used, not ptr
		*/
		send_except(std::move(*t), owner);
		break;
	}
	case Packet::Type::CHAT32: {
		auto t = Packet::deserialize<Packet::Chat32>(packet);
		
		std::cout << "chat: " << t->message << "\n";
	}
	}
}

void Server::on_quit(std::shared_ptr<TCPConnection> connection) {
	std::cout << "a player quit\n";
}