#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "network/tcp_connection.h"
#include "entity/IEntity.h"
#include "Profile.h"

class PlayerList
{
private:
	std::string filename_banned_players = "banned-players.txt";
	std::string filename_banned_ips = "banned-ips.txt";

	/*
	* online player entities
	*/
	//std::unordered_map<UUID, EntityPlayer::ptr> uuid_player_map;

	/*
	* online players by uuid
	*/
	std::unordered_map<UUID, TCPConnection::ptr> uuid_conn_map;

	//std::unordered_map<TCPConnection::ptr, 
		//EntityPlayer::ptr> conn_entity_map;

	/*
	* offline player entity data
	*/
	std::unordered_map<UUID, Profile> offline_profiles;

	uint16_t max_players = 10;

public:
	PlayerList();

public:
	/*
	* add_player(...): 
	* 	- a player has joined,
	*	- check if player new or previously logged
	*		if doesnt exist, create new
	*		if DOES exist in profiles, copy data over, then remove prof
	*/
	void add_player(TCPConnection::ptr connection);

	void save_player(UUID uuid);

	/*
	* save_lists(): save to file
	*/
	//void save_lists();

	EntityPlayer::ptr get_online_player(UUID uuid);
	Profile* get_profile(UUID uuid);

};

