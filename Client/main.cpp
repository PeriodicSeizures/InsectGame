#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "task/Task.h"
#include "engine/Engine.h"

int main(void)
{
	Engine::init();
	Task::init();

	MAIN_MENU_TASK.focus();
	//WORLD_TASK.focus();

	//std::chrono::

	//bool alive = true;
	bool render = true;
	unsigned int last_update = SDL_GetTicks();
	while (Task::GAME_ALIVE) {

		unsigned int current = SDL_GetTicks();
		float delta = (current - last_update) / 1000.f;

		// event polling thousands of times per second is
		// unnecessary and drastically increases cpu usage
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			Task::current_task->on_event(e);

			switch (e.type)
			{
			case SDL_QUIT:
				Task::GAME_ALIVE = false;
				break;
			case SDL_WINDOWEVENT:
				switch (e.window.event) {
				case SDL_WINDOWEVENT_SHOWN:
				case SDL_WINDOWEVENT_EXPOSED:
				case SDL_WINDOWEVENT_MAXIMIZED:
					//case SDL_WINDOWEVENT_ENTER:
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					//case SDL_WINDOWEVENT_TAKE_FOCUS:
					render = true;
					break;
				case SDL_WINDOWEVENT_HIDDEN:
				case SDL_WINDOWEVENT_MINIMIZED:
					//case SDL_WINDOWEVENT_LEAVE:
				case SDL_WINDOWEVENT_FOCUS_LOST:
					render = false;
					break;
				}
			}
			
		}

		Task::current_task->on_update(delta);

		// only render on mouse focus
		if (render) {
			Engine::fill({0, 0, 0, 255});

			//player.onRender();
			Task::current_task->on_render();

			Engine::doRender();
		}

		last_update = current;

	}

	Task::uninit();
	Engine::uninit();

	return 0;
}