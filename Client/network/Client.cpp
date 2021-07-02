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



static int tps = 0;
static auto last = std::chrono::steady_clock::now();




void Client::on_tick(float delta) {

	// count ticks per second

	auto now = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - last).count();
	if (diff > 1000000) {
		std::cout << "tps: " << tps << "/20, " << delta << "s\n";
		tps = 0;
		last = std::chrono::steady_clock::now();
	}
	tps++;

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


	static uint16_t last_input_mask = 0;
	uint16_t input_mask = 0;

	if (keystate[SDL_SCANCODE_W]) {
		input_mask |= Input::PRESS_UP;
	}
	else if (keystate[SDL_SCANCODE_S]) {
		input_mask |= Input::PRESS_DOWN;
	}

	if (keystate[SDL_SCANCODE_D]) {
		input_mask |= Input::PRESS_RIGHT;
	}
	else if (keystate[SDL_SCANCODE_A]) {
		input_mask |= Input::PRESS_LEFT;
	}

	/*
	* Client-side prediction using input mask
	*/
	player->input_move(input_mask);

	// send input across
	if (input_mask != last_input_mask) {

		Packet::C2SClientInput packet = {
			0,				// seq
			input_mask
		};

		send(std::move(packet));
	}

	// do something per 1/20 seconds
	for (auto&& entity : uuid_entity_map) {
		//static_cast<ClientImpl*>(entity.second->impl)->
		entity.second->on_tick(delta);
	}

	last_input_mask = input_mask;

	player->on_tick(delta);
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

	// position hud
	Engine::drawString(std::to_string(player->x) + " " + std::to_string(player->y),
		5, 5, Engine::WHITE, 1, false);

	// blit
	Engine::doRender();
}

void Client::auth_listener(Packet packet) {
	// perform listens in order, no asio needed!

}

void Client::game_listener(Packet packet) {
	// what to do when a packet is received
	switch (packet.type) {
	case Packet::Type::S2C_CLIENT_MOTION: {

		// test
		auto t = Packet::deserialize<Packet::S2CClientMotion>(packet);

		/*
		* If player sequence is greater then
		* packet sequence,
		* then 
		*/
		//if (player->input_seq < t->seq) {
		//
		//}

		/*
		* For basic client side prediction, implement:
		*/
		player->set_transform(t->x, t->y, t->vx, t->vy, t->ax, t->ay, t->angle);

		break;
	}
	case Packet::Type::S2C_ENTITY_MOTION: {
		auto t = Packet::deserialize<Packet::S2CEntityMotion>(packet);
		auto&& find = uuid_entity_map.find(t->uuid);
		LOG_DEBUG("%llu transform\n", t->uuid);
		if (find != uuid_entity_map.end()) {

			/*
			* move the uuid player
			*/
			find->second->set_transform(t->x, t->y, t->vx, t->vy, t->ax, t->ay, t->angle);
		}
		break;
	}
	case Packet::Type::S2C_PLAYER_NEW: {
		auto t = Packet::deserialize<Packet::S2CPlayerNew>(packet);
		// add to map
		LOG_DEBUG("%llu new player\n", t->uuid);
		uuid_entity_map.insert({ t->uuid,
			std::make_shared<EntityPlayer>(t->uuid, t->name, new ClientImpl()) });
		break;
	}
	case Packet::Type::S2C_ENTITY_DELETE: {
		auto t = Packet::deserialize<Packet::S2CEntityDelete>(packet);

		LOG_DEBUG("%llu delete player\n", t->uuid);

		// delete from entities
		uuid_entity_map.erase(t->uuid);

		break;
	}
	case Packet::Type::S2C_CLIENT_IDENTITY: {
		auto t = Packet::deserialize<Packet::S2CClientIdentity>(packet);

		std::cout << t->uuid << " id\n";
		LOG_DEBUG("%llu identity\n", t->uuid);

		this->player->uuid = t->uuid;

		break;
	}

	}
}