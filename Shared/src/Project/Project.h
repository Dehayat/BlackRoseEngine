#pragma once
#include <list>
#include <string>

#include <ryml/ryml.hpp>

class Project {
	std::list<std::string> pksFiles;
	std::list<std::string> levelFiles;
	int startLevel;

public:
	Project(ryml::NodeRef& node);
	Project();
	void Serialize(ryml::NodeRef& node);
	void AddPackage(std::string file);
	void RemovePackage(std::string file);
	void AddLevel(std::string file);
	void RemoveLevel(std::string file);
	void SetStartLevel(int level);
	const int GetStartLevel() const;
	const std::list<std::string>& GetPkgFiles() const;
	std::string GetPkgFile(int index) const;
	const std::list<std::string>& GetLevelFiles() const;
	std::string GetLevelFile(int index) const;
};