/*
	Packet based UDP Server
*/

#include <stdio.h>
#include <iostream>
#include <string>
#include "task/Task.h"
#include "network/tcp_connection.h"
#include "network/tcp_server.h"

int main()
{
	{
		//TCPServer server(13);

		//server.start();

		Task task;

		Task::server.start();

		/*
		* poll incoming packets on main thread
		*/
		std::thread tick_thread([&task]() {
			while (true) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				//task.
				task.on_tick();
			}
		});

		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			task.on_update(0.02f);
			//task.
		}

		TCPServer::run_thread.join();

		return 0;
	}
}