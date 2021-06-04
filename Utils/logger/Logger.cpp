#include "Logger.h"
//#include <ctime>
#include <string>
#include <windows.h>

namespace Logger
{

	static FILE* f;
	static HANDLE hConsole;

	void init() {
		time_t t = time(0);
		tm* now = localtime(&t);

		std::string filename = std::to_string(now->tm_year + 1900) + "-" + 
			std::to_string(now->tm_mon + 1) + "-" + 
			std::to_string(now->tm_mday) + "_" + 
			std::to_string(now->tm_hour) + "-" + 
			std::to_string(now->tm_min) + "-" + 
			std::to_string(now->tm_sec) + "_log.txt";

		f = fopen(filename.c_str(), "w");

		if (!f) {
			printf("unable to open output log file\n");
			//return;
		}

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	void uninit() {
		fclose(f);
	}

	void write(std::string s) {
		fwrite(s.c_str(), 1, s.size(), f);
		//fwrite("\n", 1, 1, f);
	}

	void info(std::string s) {
		time_t t = time(0);
		tm *now = localtime(&t);

		std::string out = std::string("[") +
			std::to_string(now->tm_hour) + ":" +
			std::to_string(now->tm_min) + ":" +
			std::to_string(now->tm_sec) + "] " + s + "\n";

		SetConsoleTextAttribute(hConsole, 7); // 7

		std::cout << out;

		write(out);
	}

	void important(std::string s) {
		time_t t = time(0);
		tm* now = localtime(&t);

		std::string out = std::string("[") +
			std::to_string(now->tm_hour) + ":" +
			std::to_string(now->tm_min) + ":" +
			std::to_string(now->tm_sec) + "] " + s + "\n";

		SetConsoleTextAttribute(hConsole, 3); // 3

		std::cout << out;

		write(out);
	}

	void error(std::string s) {
		time_t t = time(0);
		tm* now = localtime(&t);

		std::string out = std::string("[") +
			std::to_string(now->tm_hour) + ":" +
			std::to_string(now->tm_min) + ":" +
			std::to_string(now->tm_sec) + "] " + s + "\n";

		SetConsoleTextAttribute(hConsole, 4); //12

		std::cout << out;

		write(out);
	}

};
