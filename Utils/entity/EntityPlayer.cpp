#include "IEntity.h"


EntityPlayer::EntityPlayer(UUID uuid, std::string name, EntityImpl* impl)
	: name(name), IEntity(uuid, impl)
{

}

void EntityPlayer::on_tick(float delta) {
	
	IEntity::on_tick(delta);

}

Packet EntityPlayer::packet_transform() {

	Packet::S2CClientMotion m = {

	};

	return Packet::serialize(m);
}

Packet EntityPlayer::packet_new() {
	Packet::S2CPlayerNew p = { 
		uuid
	};

	int size = name.size();
	if (size > 16) size = 16;
	std::memcpy(p.name, name.c_str(), size);

	return Packet::serialize(std::move(p));
}

void EntityPlayer::input_move(uint16_t m) {
	if ((m & Input::PRESS_UP) == Input::PRESS_UP) {
		//fy = -20;
		ay = -700;
	}
	else if ((m & Input::PRESS_DOWN) == Input::PRESS_DOWN) {
		//fy = 20;
		ay = 700;
	}
	else {
		//fy = 0;
		ay = 0;
	}

	if ((m & Input::PRESS_LEFT) == Input::PRESS_LEFT) {
		//fx = -20;
		ax = -700;
	}
	else if ((m & Input::PRESS_RIGHT) == Input::PRESS_RIGHT) {
		//fx = 20;
		ax = 700;
	}
	else {
		//fx = 0;
		ax = 0;
	}
}