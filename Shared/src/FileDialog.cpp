#include "FileDialog.h"
#include <filesystem>
#include <nfd.h>
#include "FileDialog.h"

FileDialog::FileDialog()
{
	workingPath = std::filesystem::current_path().string();
}

FileDialog::~FileDialog()
{
}

std::string FileDialog::SaveFile(const std::string& extension, const std::string& path)
{
	nfdchar_t* fileName = NULL;
	nfdresult_t result = NFD_ERROR;
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

std::string FileDialog::OpenFile(const std::string& extension, const std::string& path)
{
	nfdchar_t* fileName = NULL;
	nfdresult_t result = NFD_ERROR;
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

std::string FileDialog::GetRelativePath(const std::string& root, const std::string& path)
{
	auto rootPath = std::filesystem::path(root);
	auto childPath = std::filesystem::path(path);
	return std::filesystem::relative(path, rootPath).string();
}

std::string FileDialog::GetFileExtension(const std::string& fileName)
{
	auto filePath = std::filesystem::path(fileName);
	return filePath.extension().string();
}
std::string FileDialog::GetFileDirectory(const std::string& fileName)
{
	auto filePath = std::filesystem::path(fileName);
	return filePath.parent_path().string();
}


