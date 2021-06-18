#include <cmath>
#include <limits>
#include "IEntity.h"
#include "../network/Packet.h"

IEntity::IEntity(UUID uuid, EntityImplement* impl)
	: uuid(uuid), impl(impl) {}

void IEntity::on_physics() {
	x_prev = x;
	y_prev = y;
	ax_prev = ax;
	ay_prev = ay;

	vx += ax * 0.05f; // increment the velocity in tick
	vy += ay * 0.05f;

	// terminal velocity
	if (vx > max_speed)
		vx = max_speed;
	else if (vx < -max_speed)
		vx = -max_speed;

	if (vy > max_speed)
		vy = max_speed;
	else if (vy < -max_speed)
		vy = -max_speed;

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



	x += vx * 0.05f; // increment the position in tick
	y += vy * 0.05f;

	// velocity and position are predictable
	// but accelerations can happen at any moment
	// due to player input
	//if (ax_prev != ax || ay_prev != ay) {
	//	Packet::Transform transform = { uuid, x, y, vx, vy, ax, ay, angle };
	//	SERVER->send_all(std::move(transform));
	//}
	
}

void IEntity::on_tick() {
	// do physics
	on_physics();

	// velocity and position are predictable
	// but accelerations can happen at any moment
	// due to player input
	//if (ax_prev != ax || ay_prev != ay) {
	//	Packet::Transform transform = { uuid, x, y, vx, vy, ax, ay, angle };
	//	CLIENT->send(std::move(transform));
	//}
}

void IEntity::set_transform(float x, float y,
	float vx, float vy,
	float ax, float ay,
	float angle) {
	x_prev = this->x;
	y_prev = this->y;
	ax_prev = this->ax;
	ay_prev = this->ay;

	this->x = x;
	this->y = y;
	this->vx = vx;
	this->vy = vy;
	this->ax = ax;
	this->ay = ay;
	this->angle = angle;
}

//void IEntity::setImpl(EntityImplement* impl) {
//	this->impl = impl;
//}