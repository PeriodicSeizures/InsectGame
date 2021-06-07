#include "Task.h"
#include "../entity/IEntity.h"

WorldTask WORLD_TASK;

Engine::Sprite* WorldTask::brick_sprite;

WorldTask::WorldTask() {
	// queues sprite for loading
	//Task::sprites_to_load.insert(
	//	{ "resources/brick.json", &this->brick_sprite });
}

void WorldTask::on_render() {
	// draw the world tiles
	for (float x = 0; x < 28; x++) {
		for (float y = -20; y < 3; y++) {
			//Engine::
			brick_sprite->draw(x * 32.f, y * 32.f, 0);
		}
	}

	for (auto&& entity : this->entities) {
		entity.second->render();
	}

	if (client)
		client->render();
}

void WorldTask::on_update(float delta) {
	// entity updates for smoothness until new packet update
	for (auto&& pair : this->entities) {
		pair.second->physics(delta);
	}

	if (client) {
		client->handle_input(delta);
		client->physics(delta);
	}
}

void WorldTask::on_tick() {

	// process in packets every 1/20 of a second (incrementally)
	process_packets();

}

void WorldTask::on_event(SDL_Event& e) {
	


}

void WorldTask::process_packets() {

	AsyncQueue<Packet>& in = Task::connection->in_packets;

	uint16_t processed = 10;
	while (Task::connection->is_read_ready() && processed--) {

		Packet packet = in.pop_front();

		switch (packet.type) {
		case Packet::Type::TRANSFORM: {
			Packet::Transform* p = 
				Packet::deserialize<Packet::Transform>(packet);
			
			auto&& find = entities.find(p->target);
			if (find != entities.end()) {
				find->second->set_transform(p->x, p->y,
					p->vx, p->vy,
					p->ax, p->ay,
					p->angle);
			}
			break;
		} 
		case Packet::Type::SRC_SERVER_ENTITY_NEW: {
			Packet::EntityNew* p = 
				Packet::deserialize<Packet::EntityNew>(packet);

			IEntity* new_entity = IEntity::create(p->type, p->uuid, p->name);

			auto&& find = entities.find(p->uuid);
			if (find != entities.end()) {
				// remove the prior entity
				delete find->second;				
			}
			// add the new entity
			entities.insert({ p->uuid, new_entity });			

			break;
		} 
		case Packet::Type::SRC_SERVER_CLIENT_IDENTITY: {
			Packet::ClientIdentity* p =
				Packet::deserialize<Packet::ClientIdentity>(packet);

			delete client;

			// upon this receive, set my identity
			switch (p->type) {
			case Entity::Type::ANT:
				client = new EntityPlayerAnt(p->uuid, "crzi");
				break;
			case Entity::Type::SPIDER:
				client = new EntityPlayerSpider(p->uuid, "crzi");
				break;
			}

			break;
		}
		}

	}

	
}