#include <cmath>
#include <limits>
#include "IEntity.h"
#include "../network/Packet.h"

IEntity::IEntity(UUID uuid, EntityImpl* impl)
	: uuid(uuid), impl(impl) {}

IEntity::~IEntity() {
	//assert(impl);
	delete impl;
	std::cout << "~IEntity()\n";
}

inline float sign(float f) {
	return f > 0.f ? 1.f : f < 0.f ? -1.f : 0.f;
}

void IEntity::on_physics(float delta) {
	x_prev = x;
	y_prev = y;
	ax_prev = ax;
	ay_prev = ay;

	/*
	* calculate forces
	*/

	//// constant normal force
	//static constexpr float force_n = force_g * mass;
	//
	//// constant frictional force
	//static constexpr float force_f = fric * force_n;
	//
	//// final force
	////float force_x = (fx) - sign(fx)*force_f;
	////float force_y = (fy) - sign(fy)*force_f;
	//
	//// mass * a = force_f
	//ax = (fx - sign(fx) * force_f) / mass;
	//ay = (fy - sign(fy) * force_f) / mass;
	//
	////ax = (force_x + force_f) / mass;
	////ay = (force_y + force_f) / mass;

	/*
	* calculate velocities
	*/

	// increment the velocity in tick
	vx += ax * delta;
	vy += ay * delta;

	// terminal velocity
	if (vx > max_speed)
		vx = max_speed;
	else if (vx < -max_speed)
		vx = -max_speed;

	if (vy > max_speed)
		vy = max_speed;
	else if (vy < -max_speed)
		vy = -max_speed;

	/*
	* friction is actually an acceleration in the opposite direction
	* due to the normal force and gravity and fric coeff
	* touch
	*	- a correct implementation:
	*	- apply a negative acceleration, equal to 
	*/

	// friction when no work is being applied is
	// a negative acceleration (opposite to vel)
	//ax *= -fric;



	if (abs(ax) < std::numeric_limits<float>::epsilon()) {
		//if (vx < 0) vx *= fric / delta;
		//else vx *= -fric / delta;
		if (vx < 0) vx += fric*delta;
		else vx += -fric*delta;
	
		if (abs(vx) <= fric) // fric elipson
			vx = 0;
	}
	
	if (abs(ay) < std::numeric_limits<float>::epsilon()) {
		//if (vy < 0) vy *= fric / delta;
		//else vy *= -fric / delta;
		if (vy < 0) vy += fric*delta;
		else vy += -fric*delta;
	
		if (abs(vy) <= fric) // fric elipson
			vy = 0;
	}



	x += vx * delta; // increment the position in tick
	y += vy * delta;

	// velocity and position are predictable
	// but accelerations can happen at any moment
	// due to player input
	//if (ax_prev != ax || ay_prev != ay) {
	//	Packet::Transform transform = { uuid, x, y, vx, vy, ax, ay, angle };
	//	SERVER->send_all(std::move(transform));
	//}
	
}

void IEntity::on_tick(float delta) {
	// do physics
	on_physics(delta);

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

Packet IEntity::packet_transform() {

	// send
	Packet::S2CEntityMotion transform = { 
		uuid, 
		x,
		y,
		vx,
		vy,
		ax,
		ay,
		angle};

	return Packet::serialize(std::move(transform));
	//conn->send(std::move(transform));

}

Packet IEntity::packet_delete() {

	Packet::S2CEntityDelete p = {
		uuid
	};

	return Packet::serialize(std::move(p));
}