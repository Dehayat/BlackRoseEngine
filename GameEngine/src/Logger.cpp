#include "Logger.h"
#include <iostream>


void Logger::Log(const std::string& message)
{
#ifdef _DEBUG


	std::string msg = "LOG: " + message;
	std::clog << "\033[0m" << msg << "\033[0m" << std::endl;

#endif // DEBUG
}

void Logger::Error(const std::string& message)
{
#ifdef _DEBUG


	std::string msg = "ERR: " + message;
	std::cerr << "\x1B[91m" << msg << "\033[0m" << std::endl;
#endif // DEBUG
}
