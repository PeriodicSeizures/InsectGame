#include <cmath>
#include <limits>
#include "Player.h"
#include "task/Task.h"

void PlayerEntity::on_tick() {

	x_prev = x;
	y_prev = y;

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

	// then packet update
	if (x_prev != x || y_prev != y) {

	}
}