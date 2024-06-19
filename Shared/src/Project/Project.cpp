#include "Project.h"

#include <ryml/ryml_std.hpp>

#include "../Core/Log.h"

Project::Project(ryml::NodeRef& node)
{
	startLevel = -1;
	if (node.has_child("Levels")) {
		auto levels = node["Levels"];
		auto child = levels.first_child();
		for (int i = 0; i < levels.num_children(); i++) {
			std::string levelName;
			child >> levelName;
			AddLevel(levelName);
		}
	}
	if (node.has_child("Packages")) {
		auto pkgs = node["Packages"];
		auto child = pkgs.first_child();
		for (int i = 0; i < pkgs.num_children(); i++) {
			std::string pkgName;
			child >> pkgName;
			AddPackage(pkgName);
		}
	}
	if (node.has_child("StartLevel")) {
		node["StartLevel"] >> startLevel;
	}
}

Project::Project()
{
	startLevel = -1;
}

void Project::Serialize(ryml::NodeRef& node)
{
	node |= ryml::MAP;
	auto pkgs = node.append_child();
	pkgs.set_key("Packages");
	pkgs |= ryml::SEQ;
	for (auto& pkg : this->pksFiles) {
		auto child = pkgs.append_child();
		child << pkg;
	}

	auto levels = node.append_child();
	levels.set_key("Levels");
	levels |= ryml::SEQ;
	for (auto& level : this->levelFiles) {
		auto child = levels.append_child();
		child << level;
	}
	if (startLevel != -1) {
		node["StartLevel"] << startLevel;
	}
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
