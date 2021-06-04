#include <stdint.h>
#include <cmath>
#include <limits>
#include "IEntity.h"

IEntity::IEntity(UUID uuid)
	: x(0), y(0), vx(0), vy(0), ax(0), ay(0), uuid(uuid) { }

void IEntity::physics(float delta) {

	vx += ax * delta; // increment the velocity in tick
	vy += ay * delta;

	// terminal velocity
	if (vx > terminal_v)
		vx = terminal_v;
	else if (vx < -terminal_v)
		vx = -terminal_v;

	if (vy > terminal_v)
		vy = terminal_v;
	else if (vy < -terminal_v)
		vy = -terminal_v;

	// friction falloff
	// if the player is being moved by player
	if (abs(ax) < std::numeric_limits<float>::epsilon()) {
		if (vx < 0) vx += fric;
		else vx += -fric;

		if (abs(vx) <= fric) // fric elipson
			vx = 0;
	}

	if (abs(ay) < std::numeric_limits<float>::epsilon()) {
		if (vy < 0) vy += fric;
		else vy += -fric;

		if (abs(vy) <= fric) // fric elipson
			vy = 0;
	}



	x += vx * delta; // increment the position in tick
	y += vy * delta;

}

void IEntity::render() {

	if (sprite->animations[cur_anim].durations.size() > 1) {
		Engine::Animation& anim = sprite->animations[cur_anim];
		uint32_t ticks = SDL_GetTicks();

		auto t = (ticks - cur_time);
		if (t >= anim.durations[cur_frame] && do_animate) {
			(++cur_frame) %= anim.durations.size() - 1;
			cur_time = ticks;
		}
	}

	sprite->draw(x, y, cur_frame, cur_anim, angle);
}

void IEntity::set_transform(float x, float y, float vx, float vy, float ax, float ay, float angle) {
	x_prev = this->x;
	y_prev = this->y;
	vx_prev = this->vx;
	vy_prev = this->vy;
	ax_prev = this->ax;
	ay_prev = this->ay;
	angle_prev = this->angle;

	this->x = x;
	this->y = y;
	this->vx = vx;
	this->vy = vy;
	this->ax = ax;
	this->ay = ay;
	this->angle = angle;
}

IEntity *IEntity::create(Entity::Type type, UUID uuid, std::string name) {
	switch (type) {
	case Entity::Type::ANT:
		return new EntityPlayerAnt(uuid, name);
	case Entity::Type::SPIDER:
		return new EntityPlayerSpider(uuid, name);
	//case Entity::Type::SPIDER:
//		return new EntityPlayerSpider(uuid, name);
	}
	return nullptr;
}