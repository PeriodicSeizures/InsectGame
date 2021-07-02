#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "engine/Engine.h"
#include "network/Client.h"

int main(void)
{
	Engine::init();
	CLIENT = new Client();

	// temp connection test
	//CLIENT->connect("192.168.1.104", "13");

	CLIENT->start();	

	delete CLIENT;
	Engine::uninit();

	return 0;
}