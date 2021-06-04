/*
	Packet based UDP Server
*/

#include <stdio.h>
#include <iostream>
#include <string>
//#include ""
#include "network/tcp_connection.h"
#include "network/tcp_server.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

int main()
{
	{
		TCPServer server(13);

		server.start();

		/*
		* poll incoming packets on main thread
		*/
		while (true) {
			server.tick();
		}

		TCPServer::run_thread.join();

		return 0;
	}
}