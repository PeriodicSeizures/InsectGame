#include "IEntity.h"

Engine::Sprite* EntityPlayerSpider::sprite;

EntityPlayerSpider::EntityPlayerSpider(UUID uuid, std::string name)
	: EntityClientController(uuid, name) {
	this->sprite = EntityPlayerSpider::sprite;
}