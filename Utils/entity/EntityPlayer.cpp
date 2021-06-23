#include "IEntity.h"


EntityPlayer::EntityPlayer(UUID uuid, std::string name, EntityImpl* impl)
	: name(name), IEntity(uuid, impl)
{

}

void EntityPlayer::on_tick() {
	
	IEntity::on_tick();

}



Packet EntityPlayer::packet_new() {
	Packet::PlayerNew p = { 
		uuid
	};

	int size = name.size();
	if (size > 16) size = 16;
	std::memcpy(p.name, name.c_str(), size);

	return Packet::serialize(std::move(p));
}