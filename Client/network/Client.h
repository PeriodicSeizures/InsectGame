#pragma once

#include <SDL.h>
#include "../engine/Engine.h"
#include "TCPClient.h"
#include "entity/IEntity.h"

class Client : public TCPClient {

private:
	std::unordered_map<UUID, EntityPlayer> entities;

	EntityPlayer *player;

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
	void on_packet(Packet) override;

};

extern Client* CLIENT;