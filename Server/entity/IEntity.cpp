#include <stdint.h>
#include <cmath>
#include <limits>
#include "IEntity.h"
#include "../task/Task.h"

IEntity::IEntity(UUID uuid)
	: x(0), y(0), vx(0), vy(0), ax(0), ay(0), uuid(uuid) { }

// called on update interval
void IEntity::physics(float delta) {

	/*
	* update prev
	*/

	x_prev = this->x;
	y_prev = this->y;
	vx_prev = this->vx;
	vy_prev = this->vy;
	ax_prev = this->ax;
	ay_prev = this->ay;
	angle_prev = this->angle;

	/*
	* now physics
	*/

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

// sends packets
void IEntity::on_tick() {
	// inexpensive check
	if (x_prev != x || y_prev != y) {
		// send packet
		Packet::Transform p = {uuid, x, y, vx, vy, ax, ay, angle};
		Task::server.dispatch_except(std::move(p), uuid);
	}
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