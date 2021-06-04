#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "Common.h"
#include "../engine/Engine.h"
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

	Engine::Sprite *sprite; // pointer to a sprite

	bool do_animate = true;
	uint8_t cur_frame;	// the current frame
	uint8_t cur_anim;	// the current animation
	uint32_t cur_time;	// last frame blit time

	static constexpr float terminal_v = 150.f;
	static constexpr float fric = 4.f;
	static constexpr bool is_kinematic = true;

public:
	IEntity(UUID uuid);

public:
	// used by both server and client
	virtual void physics(float delta);

	virtual void render();

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
class EntityClientController : public IEntity {
protected:
	std::string name;

public:
	EntityClientController(UUID uuid, std::string name);

	// should be called for only 1 object ever (the playable client)
	void handle_input(float delta);
};

class EntityPlayerAnt : public EntityClientController {
public:
	static Engine::Sprite *sprite;
	EntityPlayerAnt(UUID uuid, std::string name);

};

class EntityPlayerSpider : public EntityClientController {
public:
	static Engine::Sprite* sprite;
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