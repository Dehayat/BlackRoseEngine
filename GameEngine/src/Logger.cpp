#include "Logger.h"
#include <iostream>


void Logger::Log(const std::string& message)
{
	std::string msg = "LOG: " + message;
	std::cerr << "\033[0m" << msg << "\033[0m" << std::endl;
}

void Logger::Error(const std::string& message)
{
	std::string msg = "ERR: " + message;
	std::cerr << "\x1B[91m" << msg << "\033[0m" << std::endl;
}
