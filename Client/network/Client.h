#pragma once

#include <SDL.h>
#include "../engine/Engine.h"
#include "TCPClient.h"
#include "entity/IEntity.h"



class Client : public TCPClient {

private:
	std::unordered_map<UUID, IEntity::ptr> uuid_entity_map;

	uint32_t num_packets;

	EntityPlayer::ptr player;

public:
	Client();
	~Client();

private:
	/*
	* Overridden methods
	*/
	void on_tick() override;
	void on_render() override;

	/*
	* events
	*/
	void auth_listener(Packet);

	void game_listener(Packet);




};

extern Client* CLIENT;