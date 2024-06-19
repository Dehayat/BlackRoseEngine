#pragma once
#include <string>

#include "../AssetPipline/AssetStore.h"
#include "Project.h"

class ProjectLoader
{
public:
	ProjectLoader();
	~ProjectLoader();
	Project* LoadProject(const std::string& fileName);
	Project* NewProject(const std::string& fileName);
	void SaveProject();
	void UnloadProject();
	const std::string& GetCurrentProjectFile();
	Project* GetCurrentProject();
private:
	std::string loadedProjectPath;
	Project* loadedProject;
	AssetStore* assetStore;
};