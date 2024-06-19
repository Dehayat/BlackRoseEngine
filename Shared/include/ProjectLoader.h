#pragma once
#include <string>

#include "Project.h"

class ProjectLoader
{
public:
	ProjectLoader();
	Project* LoadProject(const std::string& fileName);
	Project* NewProject(const std::string& fileName);
	void SaveProject();
	void UnloadProject();
	const std::string& GetCurrentProjectFile();
	Project* GetCurrentProject();
private:
	std::string loadedProjectPath;
	Project* loadedProject;
};