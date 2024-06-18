#pragma once
#include <string>

class FileDialog {
	std::string workingPath = "";
public:
	FileDialog();
	~FileDialog();
	std::string SaveFile(const std::string& extension, const std::string& path = "");
	std::string OpenFile(const std::string& extension, const std::string& path = "");
};