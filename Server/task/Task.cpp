#include "Task.h"

//TCPServer Task::server(13);

Task MAIN_TASK;

void Task::on_tick() {

	auto&& connections = Task::server.connections;

	for (auto it = connections.cbegin(); it != connections.cend(); ) {

		if (!it->second) {
			it = connections.erase(it);
			continue;
		}

		auto conn = it->second;

		/*
		* 
		*			PROCESS INCOMING PACKETS
		* 
		*/

		uint8_t limit = 5; // Packet limit
		while (!conn->in_packets.empty() && limit--) {
			auto packet = conn->in_packets.pop_front();

			switch (packet.type) {
			case Packet::Type::TRANSFORM: {
				// update the entities data

				Task::server.forward_except(packet, it->first);
				break;
			}
			}
		}

		++it;
	}
}