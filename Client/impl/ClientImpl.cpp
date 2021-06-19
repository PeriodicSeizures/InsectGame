#include "ClientImpl.h"

ClientImpl::ClientImpl() {

}

void ClientImpl::on_render(float x, float y,
	float vx, float vy, 
	float ax, float ay) {


	//void EntityPlayer::on_tick() {
	//	// do physics
	//	on_physics();
	//
	//	// velocity and position are predictable
	//	// but accelerations can happen at any moment
	//	// due to player input
	//	if (ax_prev != ax || ay_prev != ay) {
	//		Packet::Transform transform = { uuid, x, y, vx, vy, ax, ay, angle };
	//		CLIENT->send(std::move(transform));
	//	}
	//}


	// interpolate the position to
	// be smooth with rendering
	// kinematcs are updated every 1/20 s
	//



	SDL_Color color = { 200, 20, 200 };
	Engine::fillRect(color, x, y, 32, 32);

	//if (sprite->animations[cur_anim].durations.size() > 1) {
	//	Engine::Animation& anim = sprite->animations[cur_anim];
	//	uint32_t ticks = SDL_GetTicks();
	//
	//	auto t = (ticks - cur_time);
	//	if (t >= anim.durations[cur_frame] && do_animate) {
	//		(++cur_frame) %= anim.durations.size() - 1;
	//		cur_time = ticks;
	//	}
	//}
	//
	//sprite->draw(x, y, cur_frame, cur_anim, angle);
	
}