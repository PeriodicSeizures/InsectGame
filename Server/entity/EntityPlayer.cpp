#include "IEntity.h"
#include "../task/Task.h"

EntityPlayer::EntityPlayer(UUID uuid, std::string name)
	: IEntity(uuid) {}

// call on an update interval
void EntityPlayer::verify(float delta) {
	//avg_displacement_prev = avg_displacement;

	float sq = (x - x_prev) * (x - x_prev) +
		(y - y_prev) * (y - y_prev);

	sqdisplace += sq;
	time_elapsed += delta;

	// 1.1f for edge latency cases
	if (sqdisplace/delta > terminal_v * terminal_v * 1.1f) {
		// then movement might be exploited
		std::cout << "exploited client movement\n";
	}
}