#include "IEntity.h"

Engine::Sprite* EntityPlayerAnt::sprite;

EntityPlayerAnt::EntityPlayerAnt(UUID uuid, std::string name) 
	: EntityClientController(uuid, name) {
	this->sprite = EntityPlayerAnt::sprite;
}

