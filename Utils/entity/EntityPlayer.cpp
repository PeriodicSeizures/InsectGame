#include "IEntity.h"


EntityPlayer::EntityPlayer(UUID uuid, std::string name, EntityImpl* impl)
	: name(name), IEntity(uuid, impl)
{

}

void EntityPlayer::on_tick() {
	
	IEntity::on_tick();

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
		ay = -700;
	}
	else if ((m & Input::PRESS_DOWN) == Input::PRESS_DOWN) {
		ay = 700;
	}
	else {
		ay = 0;
	}

	if ((m & Input::PRESS_LEFT) == Input::PRESS_LEFT) {
		ax = -700;
	}
	else if ((m & Input::PRESS_RIGHT) == Input::PRESS_RIGHT) {
		ax = 700;
	}
	else {
		ax = 0;
	}
}