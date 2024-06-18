#include "FileDialogue.h"
#include <nfd.h>

std::string FileDialogue::SaveFile(const std::string& extension, const std::string& path)
{
	nfdchar_t* fileName = NULL;
	nfdresult_t result;
	if (path == "") {
		result = NFD_SaveDialog(extension.c_str(), workingPath.c_str(), &fileName);
	}
	else {
		result = NFD_SaveDialog(extension.c_str(), path.c_str(), &fileName);
	}
	if (result == NFD_OKAY) {
		std::string fileNameStr = fileName;
		if (fileNameStr.find("." + extension) == -1) {
			fileNameStr += "." + extension;
		}
		workingPath = fileNameStr.substr(0, fileNameStr.find_last_of("\\/"));
		return fileNameStr;
	}
	else {
		return "";
	}
}

std::string FileDialogue::OpenFile(const std::string& extension, const std::string& path)
{
	nfdchar_t* fileName = NULL;
	nfdresult_t result;
	if (path == "") {
		result = NFD_OpenDialog(extension.c_str(), workingPath.c_str(), &fileName);
	}
	else {
		result = NFD_OpenDialog(extension.c_str(), path.c_str(), &fileName);
	}
	if (result == NFD_OKAY) {
		std::string fileNameStr = fileName;
		workingPath = fileNameStr.substr(0, fileNameStr.find_last_of("\\/"));
		return fileNameStr;
	}
	else {
		return "";
	}
}
