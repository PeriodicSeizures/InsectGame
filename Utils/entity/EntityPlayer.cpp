#include "IEntity.h"


EntityPlayer::EntityPlayer(UUID uuid, std::string name, EntityImplement* impl)
	: name(name), IEntity(uuid, impl)
{

}

void EntityPlayer::on_tick() {

	IEntity::on_tick();

}