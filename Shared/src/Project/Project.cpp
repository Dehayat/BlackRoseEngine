#include "Project.h"

#include <set>
#include <filesystem>

#include <ryml/ryml_std.hpp>

#include "../Core/Log.h"
#include "../Core/Systems.h"
#include "../FileDialog.h"

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
			child = child.next_sibling();
		}
	}
	if (node.has_child("Packages")) {
		auto pkgs = node["Packages"];
		auto child = pkgs.first_child();
		for (int i = 0; i < pkgs.num_children(); i++) {
			std::string pkgName;
			child >> pkgName;
			AddPackage(pkgName);
			child = child.next_sibling();
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
	std::set<std::string> used;
	node |= ryml::MAP;
	auto pkgs = node.append_child();
	pkgs.set_key("Packages");
	pkgs |= ryml::SEQ;
	for (auto& pkg : this->pksFiles) {
		if (used.find(pkg) == used.end()) {
			auto child = pkgs.append_child();
			child << pkg;
			used.insert(pkg);
		}
	}
	used.clear();

	auto levels = node.append_child();
	levels.set_key("Levels");
	levels |= ryml::SEQ;
	for (auto& level : this->levelFiles) {
		if (used.find(level) == used.end()) {
			auto child = levels.append_child();
			child << level;
			used.insert(level);
		}
	}
	if (startLevel != -1) {
		node["StartLevel"] << startLevel;
	}
}

void Project::AddPackage(std::string file)
{
	//TODO: change this so it leads to project path instead
	auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), file);
	pksFiles.push_back(relativePath);
}

void Project::RemovePackage(std::string file)
{
	auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), file);
	pksFiles.remove(relativePath);
}

void Project::AddLevel(std::string file)
{
	//TODO: change this so it leads to project path instead
	auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), file);
	levelFiles.push_back(relativePath);
	if (startLevel == -1) {
		startLevel = 0;
	}
}

void Project::RemoveLevel(std::string file)
{
	auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), file);
	levelFiles.remove(relativePath);
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
