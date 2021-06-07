#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "Common.h"
#include "entity/Entity.h"

/*
* Classes which start with an I should not be instantiated
* except where derived, since there will implementations for these 
* classes
*/

/*
* Update the generic motions of the entity
* does not completely count for players
*/
class IEntity {
protected:
	UUID uuid;

	float x = 0, y = 0,
		vx = 0, vy = 0,
		ax = 0, ay = 0,
		angle = 0;

	float x_prev = 0, y_prev = 0,
		vx_prev = 0, vy_prev = 0,
		ax_prev = 0, ay_prev = 0,
		angle_prev = 0;

	static constexpr float terminal_v = 150.f;
	static constexpr float fric = 4.f;
	static constexpr bool is_kinematic = true;

public:
	IEntity(UUID uuid);

public:
	// used by both server and client
	virtual void physics(float delta);

	virtual void on_tick();

	void set_transform(float x, float y, 
		float vx, float vy,
		float ax, float ay, 
		float angle);

	static IEntity* create(Entity::Type type, UUID uuid, std::string name);
};

/*
* Server and client shared entity to derive from
* Thought of as a player other than the client on the client side (but not rendered)
*/
class EntityPlayer : public IEntity {
protected:
	std::string name;

	float sqdisplace = 0;
	float time_elapsed = 0;

public:
	EntityPlayer(UUID uuid, std::string name);

	virtual void verify(float delta);
};

class EntityPlayerAnt : public EntityPlayer {
public:
	EntityPlayerAnt(UUID uuid, std::string name);

};

class EntityPlayerSpider : public EntityPlayer {
public:
	EntityPlayerSpider(UUID uuid, std::string name);

};

//class IEntityObstacle : virtual public IEntity {
//protected:
//
//public:
//	IEntityObstacle(UUID uuid);
//
//};
//
//class IEntityProjectile : virtual public IEntity {
//protected:
//
//public:
//	IEntityProjectile();
//
//};

#endif