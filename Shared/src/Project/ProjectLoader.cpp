#include "ProjectLoader.h"

#include <sstream>

#include <ryml/ryml_std.hpp>

#include "../Core/FileResource.h"
#include "../Core/Log.h"
#include "../Core/Systems.h"

ProjectLoader::ProjectLoader()
{
	ROSE_CREATESYSTEM(AssetStore);
	assetStore = &ROSE_GETSYSTEM(AssetStore);
	loadedProject = nullptr;
	loadedProjectPath = "";
}

ProjectLoader::~ProjectLoader()
{
	if (loadedProject != nullptr) {
		UnloadProject();
	}
	assetStore = nullptr;
}

Project* ProjectLoader::LoadProject(const std::string& fileName)
{
	auto fileHandle = FileResource(fileName);
	if (fileHandle.file == nullptr) {
		ROSE_ERR("couldn't load projet");
		return nullptr;
	}
	UnloadProject();
	std::string fileString = std::string("\0", SDL_RWsize(fileHandle.file));
	SDL_RWread(fileHandle.file, &fileString[0], sizeof(fileString[0]), fileString.size());
	auto tree = ryml::parse_in_arena(ryml::to_csubstr(fileString));
	auto root = tree.rootref();
	auto project = new Project(root);
	loadedProject = project;
	loadedProjectPath = fileName;
	for (auto& pkg : loadedProject->GetPkgFiles()) {
		assetStore->LoadPackage(pkg);
	}
	return loadedProject;
}

Project* ProjectLoader::NewProject(const std::string& fileName)
{
	UnloadProject();
	loadedProject = new Project();
	loadedProjectPath = fileName;
	return loadedProject;
}

void ProjectLoader::SaveProject()
{
	auto fileHandle = FileResource(loadedProjectPath, "w+");
	if (fileHandle.file == nullptr) {
		ROSE_ERR("Couldn't save project");
		return;
	}
	auto tree = ryml::Tree();
	auto root = tree.rootref();
	loadedProject->Serialize(root);
	std::string buffer = ryml::emitrs_yaml<std::string>(tree);
	SDL_RWwrite(fileHandle.file, buffer.data(), 1, buffer.size());
}

void ProjectLoader::UnloadProject()
{
	if (loadedProject != nullptr) {
		assetStore->UnloadAllAssets();
		delete loadedProject;
		ROSE_LOG("Project %s unloaded", loadedProjectPath.c_str());
		loadedProject = nullptr;
		loadedProjectPath = "";
	}
}

const std::string& ProjectLoader::GetCurrentProjectFile()
{
	return loadedProjectPath;
}

Project* ProjectLoader::GetCurrentProject()
{
	return loadedProject;
}
