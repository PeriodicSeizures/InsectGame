#include "IEntity.h"


EntityPlayer::EntityPlayer(UUID uuid, std::string name, EntityImpl* impl)
	: name(name), IEntity(uuid, impl)
{

}

void EntityPlayer::on_tick() {
	
	IEntity::on_tick();

}