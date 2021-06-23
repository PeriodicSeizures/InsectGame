#include "Packet.h"

Packet::ErrorCode Packet::S(Type type, uint16_t &ret) {
	if ((uint16_t)type < sizeof(sizes)) {
		ret = static_cast<uint16_t>(sizes[(uint16_t)type]);
		return ErrorCode::OK;
	}
	return ErrorCode::INVALID_HEADER;
}

Packet test_serialize(Packet::Chat32 structure) {
	Packet packet = { structure.TYPE };
	if (sizeof(Packet::Chat32)) {
		packet.data.resize(sizeof(Packet::Chat32));
		std::memcpy(packet.data.data(), (void*)&structure, sizeof(Packet::Chat32));
	}
	return std::move(packet);
}