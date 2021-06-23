#include "PlayerList.h"
#include "../impl/ServerImpl.h"

PlayerList::PlayerList() {
	// load ban lists
}

void PlayerList::add_player(TCPConnection::ptr connection) {

	if (uuid_conn_map.size() < max_players) {

		//UUID &uuid = connection->uuid;
		UUID uuid = 0;
		// add player 
		auto e = std::make_shared<EntityPlayer>(uuid, "", new ServerImplPlayer());
		
		// If player previously existed, load from that
		auto&& find = offline_profiles.find(uuid);
		if (find != offline_profiles.end()) {
			e->set_transform(
				find->second.x,
				find->second.y,
				find->second.vx,
				find->second.vy,
				find->second.ax,
				find->second.ay,
				find->second.angle			
			);
			//find->second.x
			offline_profiles.erase(uuid);
		}
		
		uuid_conn_map.insert({ uuid, connection });
	}
}

void PlayerList::save_player(UUID uuid) {
	auto&& player = get_online_player(uuid);

	if (player) {
		// move player to offline
		Profile profile = {uuid, 
			player->x,
			player->y,
			player->vx,
			player->vy,
			player->ax,
			player->ay,
			player->angle};

		offline_profiles.insert({ uuid, std::move(profile) });
	}

	// remove player from online
	uuid_conn_map.erase(uuid);
}

EntityPlayer::ptr PlayerList::get_online_player(UUID uuid) {
	//auto&& find = uuid_conn_map.find(uuid);
	//if (find != uuid_conn_map.end()) {
	//	return find->second->entity;
	//}
	return nullptr;
}

Profile* PlayerList::get_profile(UUID uuid) {
	auto&& find = offline_profiles.find(uuid);
	if (find != offline_profiles.end()) {
		return &find->second;
	}
	return nullptr;
}