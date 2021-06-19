#include "ServerImpl.h"
#include "../network/Server.h"

ServerImpl::ServerImpl() {}

void ServerImpl::behaviour(IEntity* entity) {
	if (entity->ax_prev != entity->ax || 
		entity->ay_prev != entity->ay) {
		Packet::Transform transform = { 
			entity->uuid, 
			entity->x, 
			entity->y, 
			entity->vx,
			entity->vy,
			entity->ax,
			entity->ay,
			entity->angle 
		};
		SERVER->send_all(std::move(transform));
	}
}