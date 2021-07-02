#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef size_t UUID;

namespace Input {
	static constexpr uint16_t PRESS_UP		= 0b1;
	static constexpr uint16_t PRESS_RIGHT	= 0b10;
	static constexpr uint16_t PRESS_DOWN	= 0b100;
	static constexpr uint16_t PRESS_LEFT	= 0b1000;

	//static constexpr uint16_t RELEASE_H		= 0b10000;
	//static constexpr uint16_t RELEASE_V		= 0b100000;
	//static constexpr uint16_t SHOOT			= 0b1000000;
};

//enum class Input : uint16_t {
//	PRESS_UP = 0b1, 
//	PRESS_RIGHT = 0b10, 
//	PRESS_DOWN = 0b100, 
//	PRESS_LEFT = 0b1000,
//	RELEASE_UP = 0b10000, 
//	
//	RELEASE_RIGHT = 0b100000, 
//	RELEASE_DOWN = 0b1000000, 
//	RELEASE_LEFT = 0b10000000,
//	SHOOT = 0b100000000
//};

//enum class EntityType : uint16_t {
//	ANT, SPIDER, MILLIPEDE, SLIME_PUDDLE
//};

//#define DO_LOG_DEBUG

void precise_sleep(double seconds);

// https://stackoverflow.com/questions/679979/how-to-make-a-variadic-macro-variable-number-of-arguments
#ifdef DO_LOG_DEBUG
	#define LOG_DEBUG printf
#else
	#define LOG_DEBUG //
#endif // DO_LOG_DEBUG

//#define LOG_ERROR printf

#endif
