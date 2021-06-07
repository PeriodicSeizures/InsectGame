#include "IEntity.h"
#include "../task/Task.h"

static constexpr uint8_t UP_MASK	= 0b0001;
static constexpr uint8_t RIGHT_MASK = 0b0010;
static constexpr uint8_t DOWN_MASK	= 0b0100;
static constexpr uint8_t LEFT_MASK	= 0b1000;

static constexpr double ANGLES[] = {
	-999, 90, 0, 45, -90, -999, -45, -999, -180, 135, -999, -999, -135
};

EntityClientController::EntityClientController(UUID uuid, std::string name)
	: IEntity(uuid) {}

void EntityClientController::handle_input(float delta) {

	//SDL_PumpEvents();
	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	this->do_animate = false;

	uint8_t mask = 0;
	if (keystate[SDL_SCANCODE_W]) {
		do_animate = true;
		this->ay = 700;
		mask |= UP_MASK;
	}
	else if (keystate[SDL_SCANCODE_S]) {
		do_animate = true;
		this->ay = -700;
		mask |= DOWN_MASK;
	}
	else {
		this->ay = 0;
	}

	if (keystate[SDL_SCANCODE_D]) {
		do_animate = true;
		this->ax = 700;
		mask |= RIGHT_MASK;
	}
	else if (keystate[SDL_SCANCODE_A]) {
		do_animate = true;
		this->ax = -700;
		mask |= LEFT_MASK;
	}
	else {
		this->ax = 0;
	}

	//angle = 360 - ANGLES[mask] + 90;

	auto new_angle = ANGLES[mask];

	if (new_angle != -999) {
		// then try turning
		new_angle = 360 - new_angle + 90;
		angle = new_angle;
		//auto diff = new_angle - angle;
		//auto change = diff * delta;
		//if (fabs(change) < .01)
		//	change = .01;
		////if (change > .005 && change < .5)
		//	//change = .05;
		////if (fabs(diff) < 180)
		//	//change *= -1;
		//angle += change;
	}

	//angle = fmod(angle, 360);
	//if (angle < 0)
	//	angle += 360;


	//double new_angle = 360 - ANGLES[mask];
	//
	//double ac = (angle - new_angle) * delta;
	//angle += ac;
	//
	//angle = fmod(angle, 360);
	//if (angle < 0)
	//	angle += 360;

	//IEntityBase::on_update(delta);

	if (do_animate && Task::connection) {
		// send packets
		Packet::Transform packet = { 
			0,		// uuid
			x, y,	// pos
			vx, vy, // vel
			ax, ay,	// acc
			angle	// angle
		};

		Task::connection->dispatch(std::move(packet));
	}
}

//void EntityPlayerClient::on_render() {
//
//}