#include "Client.h"
#include "../impl/ClientImpl.h"

Client* CLIENT = nullptr;

Client::Client() {
	player = std::make_shared<EntityPlayer>(0, "", new ClientImpl());

	// register
	//register_listener(std::bind(&Client::auth_listener, std::placeholders::_1, this));

	//auto f = std::bind(&Client::auth_listener, this, std::placeholders::_1);
	register_listener(std::bind(&Client::game_listener, this, std::placeholders::_1));
}

Client::~Client() {}

void Client::on_tick() {
	/*
	* take input
	*/
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			stop();
			break;
		case SDL_WINDOWEVENT:
			switch (e.window.event) {
			case SDL_WINDOWEVENT_SHOWN:
			case SDL_WINDOWEVENT_EXPOSED:
			case SDL_WINDOWEVENT_MAXIMIZED:
				//case SDL_WINDOWEVENT_ENTER:
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				//case SDL_WINDOWEVENT_TAKE_FOCUS:
				this->set_render(true);
				break;
			case SDL_WINDOWEVENT_HIDDEN:
			case SDL_WINDOWEVENT_MINIMIZED:
				//case SDL_WINDOWEVENT_LEAVE:
			case SDL_WINDOWEVENT_FOCUS_LOST:
				this->set_render(false);
				break;
			}
			break;
		//case SDL_KEYDOWN: 
		//	switch (e.key.keysym.sym) {
		//	case SDLK_c:
		//		// send a "Hello, World!" chat packet
		//		send(Packet::Chat32 {"Hello, World!", 0});
		//		break;
		//	}
		//	break;
		}


	}

	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	uint8_t mask = 0;
	if (keystate[SDL_SCANCODE_W]) {
		player->ay = -700;
	}
	else if (keystate[SDL_SCANCODE_S]) {
		player->ay = 700;
	}
	else {
		player->ay = 0;
	}

	if (keystate[SDL_SCANCODE_D]) {
		player->ax = 700;
	}
	else if (keystate[SDL_SCANCODE_A]) {
		player->ax = -700;
	}
	else {
		player->ax = 0;
	}

	//angle = 360 - ANGLES[mask] + 90;

	//auto new_angle = ANGLES[mask];

	// if acceleration changes, send it across
	if (player->ax_prev != player->ax || 
		player->ay_prev != player->ay) {

		Packet::ClientInput packet = {
			0,				// seq
			Input::SHOOT	// fill in packet (REMOVE LATER
		};

		send(std::move(packet));
	}

	// do something per 1/20 seconds
	for (auto&& entity : uuid_entity_map) {
		entity.second->on_tick();
	}

	player->on_tick();
}

void Client::on_render() {
	// do rendering here
	Engine::fill(Engine::BLACK);

	// ...
	for (auto&& entity : uuid_entity_map) {
		// call impl->on_render for all entities
		static_cast<ClientImpl*>(entity.second->impl)->
			on_render(entity.second->x,
				entity.second->y,
				entity.second->vx,
				entity.second->vy,
				entity.second->ax,
				entity.second->ay);
	}
	static_cast<ClientImpl*>(player->impl)->
		on_render(player->x, 
			player->y, 
			player->vx, 
			player->vy, 
			player->ax, 
			player->ay);
	// blit
	Engine::doRender();
}

void Client::auth_listener(Packet packet) {
	// perform listens in order, no asio needed!

}

void Client::game_listener(Packet packet) {
	// what to do when a packet is received
}