#pragma once
#include <string>
#include <vector>

class Logger {
public:
	static void Log(const std::string& message);
	static void Error(const std::string& message);
};