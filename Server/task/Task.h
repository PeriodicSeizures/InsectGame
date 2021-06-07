#pragma once

#include <unordered_map>
#include <unordered_set>
#include "Common.h"
#include "../network/tcp_server.h"
#include "../Player.h"

class Task {
public:
	std::unordered_map<UUID, PlayerEntity> entities;



	//static Task*

	//std::vector<std::shared_ptr<TCPConnection>> unclarified; // send uuid to these clients

	//std::unordered_map<UUID, PlayerClient> players;

	static TCPServer server;

	//virtual void on_update(float delta);
	virtual void on_tick();
	//virtual void process_packets();
};

extern Task MAIN_TASK;