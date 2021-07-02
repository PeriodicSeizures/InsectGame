#pragma once

#include "impl/EntityImpl.h"
//#include "../network/PlayerConnection.h"
#include "entity/IEntity.h"

class ServerImpl : public EntityImpl
{
public:
	ServerImpl();

public:
	virtual void behaviour(IEntity::ptr entity, float delta);
};

class ServerImplPlayer : public ServerImpl {
private:
	//PlayerConnection player_conn;

public:
	ServerImplPlayer();

};