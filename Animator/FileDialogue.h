#pragma once
#include<string>

class FileDialogue {
	std::string workingPath = "";
public:
	std::string SaveFile(const std::string& extension, const std::string& path = "");
	std::string OpenFile(const std::string& extension, const std::string& path = "");
};