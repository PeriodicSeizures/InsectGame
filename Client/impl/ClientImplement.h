#pragma once

#include "entity/IEntity.h"
#include "../engine/Engine.h"

class ClientImplement : public EntityImplement {
public:
	ClientImplement();

public:
	virtual void on_render(float x, float y,
		float vx, float vy,
		float ax, float ay);

};