#pragma once
#include <iostream>
namespace Logger
{
	void init();
	void uninit();

	void info(std::string s);
	void important(std::string s);
	void error(std::string s);

};

