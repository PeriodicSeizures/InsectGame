#pragma once

#include "entity/IEntity.h"

class IEntityServerSide : virtual public IEntity {
public:
	virtual void tick();
};

class IEntityServerSidePlayer : public IEntityPlayer, public IEntityServerSide {
public:
	IEntityServerSidePlayer(UUID uuid);
};