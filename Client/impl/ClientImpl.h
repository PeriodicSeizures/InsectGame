#pragma once

#include "entity/IEntity.h"
#include "../engine/Engine.h"

class ClientImpl : public EntityImpl {
public:
	ClientImpl();

public:
	virtual void on_render(float x, float y,
		float vx, float vy,
		float ax, float ay);

};