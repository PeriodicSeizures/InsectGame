#include "Task.h"

TCPServer Task::server(13);

void Task::on_update(float delta) {

	auto&& connections = Task::server.connections;

	uint8_t at = 0;
	for (auto it = connections.cbegin(); it != connections.cend(); ) {

		if (!it->second.first || !it->second.first->is_connected()) {
			it = connections.erase(it);
			continue;
		}

		auto conn = it->second.first;

		/*
		* 
		*			PROCESS INCOMING PACKETS
		* 
		*/

		uint8_t limit = 5; // Packet limit
		while (conn->is_read_ready() && --limit) {
			auto packet = it->second.first->in_packets.pop_front();

			switch (packet.type) {
			case Packet::Type::TRANSFORM: {
				// actually forward to all except the sender
				Task::server.forward_except(packet, it->first);
				break;
			}
			case Packet::Type::CHAT32: {
				// instead of copying the contents of the packet data
				// into the real packet, reinterpret as that type, explicitly
				Packet::Chat32* chat = Packet::deserialize<Packet::Chat32>(packet);

				// If the target is non zero, its a private message
				if (chat->target) {
					auto&& find = entities.find(chat->target);
					if (find != entities.end()) {
						Task::server.forward(packet, chat->target);
						Task::server.forward(packet, it->first);
					}
				}
				else {
					Task::server.forward(packet);
				}

				std::cout << "chat32: " << chat->message << "\n";
				break;
			} case Packet::Type::CHAT64: {
				Packet::Chat32* chat = Packet::deserialize<Packet::Chat32>(packet);

				if (chat->target) {
					auto&& find = entities.find(chat->target);
					if (find != entities.end()) {
						Task::server.forward(packet, chat->target);
						Task::server.forward(packet, it->first);
					}
				}
				else {
					Task::server.forward(packet);
				}

				std::cout << "chat64: " << chat->message << "\n";
				break;
			}

			}
		}

		/*
		*
		*			GAME EVENTS AND OCCURRENCES
		*
		*/

		// Send packet for identity to the client if they haven't been yet decl

		if (it->second.second.insect_type == Entity::Type::NONE) {
			//Entity::Type type = static_cast<Entity::Type>(at % (uint16_t)Entity::Type::count);
			Entity::Type type = Entity::Type::ANT;
			Packet::ClientIdentity id = { it->first, type };
			conn->dispatch(id);
			connections[it->first].second.insect_type = type;
			//Entity::Type& t = it->second.second.insect_type;
			 //= type;
		}

		at++;
		++it;
	}
}

void Task::on_tick() {

}
