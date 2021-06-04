#include "Packet.h"

Packet::ErrorCode Packet::S(Type type, uint16_t &ret) {
	if ((uint16_t)type < sizeof(sizes)) {
		ret = static_cast<uint16_t>(sizes[(uint16_t)type]);
		return ErrorCode::OK;
	}
	return ErrorCode::INVALID_HEADER;
}