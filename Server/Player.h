#pragma once

struct PlayerEntity {

	float x = 0, y = 0,
		vx = 0, vy = 0,
		ax = 0, ay = 0,
		angle = 0;

	float x_prev = 0, y_prev = 0;

	float max_speed = 150.f;
	float fric = 4.f;

	void on_tick();

};