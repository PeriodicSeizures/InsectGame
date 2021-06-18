/*
	Packet based UDP Server
*/

#include <stdio.h>
#include <iostream>
#include <string>
#include "network/Server.h"

int main()
{
	auto last =
		std::chrono::system_clock::now();
	{
		SERVER = new Server(13);

		SERVER->start();

		//SERVER->stop();

		delete SERVER;

		return 0;
	}
}