#include "Project.h"

#include "../Core/Log.h"

Project::Project()
{
	startLevel = -1;
}

void Project::AddPackage(std::string file)
{
	pksFiles.push_back(file);
}

void Project::RemovePackage(std::string file)
{
	pksFiles.remove(file);
}

void Project::AddLevel(std::string file)
{
	levelFiles.push_back(file);
	if (startLevel == -1) {
		startLevel = 0;
	}
}

void Project::RemoveLevel(std::string file)
{
	levelFiles.remove(file);
	if (levelFiles.empty()) {
		startLevel = -1;
	}
}

void Project::SetStartLevel(int level)
{
	if (level >= levelFiles.size() || level < 0) {
		ROSE_ERR("level doesn't exit: %d", level);
		return;
	}
	startLevel = level;
}

const int Project::GetStartLevel() const
{
	return startLevel;
}

const std::list<std::string>& Project::GetPkgFiles() const
{
	return pksFiles;
}

std::string Project::GetPkgFile(int index) const
{
	if (index >= pksFiles.size() || index < 0) {
		ROSE_ERR("pkg doesn't exit: %d", index);
		return "";
	}
	auto it = levelFiles.begin();
	for (int i = 0; i < index; i++) {
		it++;
	}
	return *it;
}

const std::list<std::string>& Project::GetLevelFiles() const
{
	return levelFiles;
}

std::string Project::GetLevelFile(int index) const
{
	if (index >= levelFiles.size() || index < 0) {
		ROSE_ERR("pkg doesn't exit: %d", index);
		return "";
	}
	auto it = levelFiles.begin();
	for (int i = 0; i < index; i++) {
		it++;
	}
	return *it;
}
