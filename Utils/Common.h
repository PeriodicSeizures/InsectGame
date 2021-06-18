#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef size_t UUID;

enum class Input : uint16_t {
	PRESS_UP, PRESS_RIGHT, PRESS_DOWN, PRESS_LEFT,
	RELEASE_UP, RELEASE_RIGHT, RELEASE_DOWN, RELEASE_LEFT,
	SHOOT
};

//enum class EntityType : uint16_t {
//	ANT, SPIDER, MILLIPEDE, SLIME_PUDDLE
//};

#endif
