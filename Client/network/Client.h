#pragma once

#include <SDL.h>
#include "../engine/Engine.h"
#include "TCPClient.h"
#include "entity/IEntity.h"

struct SequencedInput {
	float press_time;
	uint16_t input_mask;
	int input_sequence_number;

};

class Client : public TCPClient {

private:
	std::unordered_map<UUID, IEntity::ptr> uuid_entity_map;

	/*
	* For server reconciliation
	*/
	uint16_t input_sequence_number;
	// { press_time, mask }[]
	std::vector<SequencedInput> pending_inputs;

	// time that a horizontal key has been pressed
	std::chrono::steady_clock::time_point start_h;
	// vert
	std::chrono::steady_clock::time_point start_v;



	EntityPlayer::ptr player;

public:
	Client();
	~Client();

private:
	/*
	* Overridden methods
	*/
	void on_tick(float delta) override;
	void on_render() override;

	/*
	* events
	*/
	void auth_listener(Packet);

	void game_listener(Packet);




};

extern Client* CLIENT;