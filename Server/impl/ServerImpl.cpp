#include "ServerImpl.h"
#include "../network/Server.h"

ServerImpl::ServerImpl() {}

void ServerImpl::behaviour(IEntity::ptr entity, float delta) {

	entity->on_tick(delta);
	//entity->p

	return;
	//if (entity->ax_prev != entity->ax || 
	//	entity->ay_prev != entity->ay) {
	//	Packet::S2CEntityMotion transform = { 
	//		entity->uuid, 
	//		entity->x, 
	//		entity->y, 
	//		entity->vx,
	//		entity->vy,
	//		entity->ax,
	//		entity->ay,
	//		entity->angle 
	//	};
	//	SERVER->send_all(std::move(transform));
	//}
}