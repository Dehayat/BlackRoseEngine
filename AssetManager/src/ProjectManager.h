#pragma once
#include <filesystem>

#include "Core/Log.h"
#include "FileDialog.h"
#include "AssetPipline/AssetPackage.h"

#include "Project/ProjectLoader.h"

class ProjectManager {
	std::vector<AssetPackage*> projectPackages;
	std::vector<AssetPackage*> soloPackages;
	AssetPackage* selectedProjectPkg;
	AssetPackage* selectedSoloPkg;
	AssetPackage* openPkg;
public:
	ProjectManager() {
		ROSE_CREATESYSTEM(ProjectLoader);
		ROSE_CREATESYSTEM(FileDialog);
		openPkg = nullptr;
		selectedProjectPkg = nullptr;
		selectedSoloPkg = nullptr;
	}
	~ProjectManager() {
	}

	AssetPackage* GetCurrentPackage() {
		return openPkg;
	}

	AssetPackage* CreateNewPackage(const std::string& fileName) {
		auto newPkg = new AssetPackage();
		newPkg->filePath = fileName;
		return newPkg;
	}
	AssetPackage* LoadPackage(const std::string& fileName) {
		auto pkg = new AssetPackage();
		auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), fileName);
		pkg->Load(relativePath);
		return pkg;
	}
	void UnloadPackage(AssetPackage* pkg) {
		delete pkg;
	}

	void CreateProject(const std::string& fileName) {
		for (auto pkg : projectPackages) {
			delete pkg;
		}
		projectPackages.clear();

		ROSE_GETSYSTEM(ProjectLoader).NewProject(fileName);
	}
	void LoadProject(const std::string& fileName) {
		for (auto pkg : projectPackages) {
			delete pkg;
		}
		projectPackages.clear();

		auto project = ROSE_GETSYSTEM(ProjectLoader).LoadProject(fileName);
		for (auto& pkgFile : project->GetPkgFiles()) {
			auto pkg = LoadPackage(pkgFile);
			projectPackages.push_back(pkg);
		}
	}

	void OpenPackage(AssetPackage* pkg) {
		openPkg = pkg;
	}

	void Render() {
		if (ImGui::Button("New Project")) {
			auto fileName = ROSE_GETSYSTEM(FileDialog).SaveFile("pro");
			if (fileName != "") {
				CreateProject(fileName);
			}
		}
		if (ImGui::Button("Open Project")) {
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("pro");
			if (fileName != "") {
				LoadProject(fileName);
				ROSE_GETSYSTEM(ProjectLoader).LoadProject(fileName);
			}
		}

		if (ROSE_GETSYSTEM(ProjectLoader).GetCurrentProject() != nullptr)
		{
			ImGui::SeparatorText("Project Packages");

			if (ImGui::Button("Save Project")) {
				ROSE_GETSYSTEM(ProjectLoader).SaveProject();
			}
			if (ImGui::Button("Close Project")) {
				selectedProjectPkg = nullptr;
				openPkg = nullptr;
				ROSE_GETSYSTEM(ProjectLoader).UnloadProject();
			}
			if (ImGui::BeginListBox("Project packages")) {
				for (auto pkg : projectPackages) {
					bool selected = selectedProjectPkg == pkg;
					auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), pkg->filePath);
					if (ImGui::Selectable(relativePath.c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick)) {
						selectedProjectPkg = pkg;
						if (ImGui::IsMouseDoubleClicked(0)) {
							OpenPackage(pkg);
						}
					}
				}
				ImGui::EndListBox();
			}

			if (ImGui::Button("\\/") && selectedProjectPkg != nullptr) {
				auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), selectedProjectPkg->filePath);
				ROSE_GETSYSTEM(ProjectLoader).GetCurrentProject()->RemovePackage(relativePath);
				projectPackages.erase(std::find(projectPackages.begin(), projectPackages.end(), selectedProjectPkg));
				soloPackages.push_back(selectedProjectPkg);
				if (selectedProjectPkg == openPkg) {
					openPkg = nullptr;
				}
				selectedProjectPkg = nullptr;
			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4, 0.2, 0.2, 1));
			if (ImGui::Button("Delete") && selectedProjectPkg != nullptr) {
				auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), selectedProjectPkg->filePath);
				ROSE_GETSYSTEM(ProjectLoader).GetCurrentProject()->RemovePackage(relativePath);
				projectPackages.erase(std::find(projectPackages.begin(), projectPackages.end(), selectedProjectPkg));
				UnloadPackage(selectedProjectPkg);
				if (selectedProjectPkg == openPkg) {
					openPkg = nullptr;
				}
				selectedProjectPkg = nullptr;
			}
			ImGui::PopStyleColor();
		}
		ImGui::SeparatorText("Solo Packages");
		ImGui::PushID("Solo");
		if (ROSE_GETSYSTEM(ProjectLoader).GetCurrentProject() != nullptr) {
			if (ImGui::Button("^") && selectedSoloPkg != nullptr) {
				ROSE_GETSYSTEM(ProjectLoader).GetCurrentProject()->AddPackage(selectedSoloPkg->filePath);
				soloPackages.erase(std::find(soloPackages.begin(), soloPackages.end(), selectedSoloPkg));
				projectPackages.push_back(selectedSoloPkg);
				if (selectedSoloPkg == openPkg) {
					openPkg = nullptr;
				}
				selectedSoloPkg = nullptr;
			}
			ImGui::SameLine();
		}
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4, 0.2, 0.2, 1));
		if (ImGui::Button("Delete") && selectedSoloPkg != nullptr) {
			soloPackages.erase(std::find(soloPackages.begin(), soloPackages.end(), selectedSoloPkg));
			UnloadPackage(selectedSoloPkg);
			if (selectedSoloPkg == openPkg) {
				openPkg = nullptr;
			}
			selectedSoloPkg = nullptr;
		}
		ImGui::PopStyleColor();
		if (ImGui::BeginListBox("Solo packages")) {
			for (auto pkg : soloPackages) {
				bool selected = selectedSoloPkg == pkg;
				auto relativePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), pkg->filePath);
				if (ImGui::Selectable(relativePath.c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick)) {
					selectedSoloPkg = pkg;
					if (ImGui::IsMouseDoubleClicked(0)) {
						OpenPackage(pkg);
					}
				}
			}
			ImGui::EndListBox();
		}
		if (ImGui::Button("New Asset Package")) {
			auto fileName = ROSE_GETSYSTEM(FileDialog).SaveFile("pkg");
			if (fileName != "") {
				auto pkg = CreateNewPackage(fileName);
				soloPackages.push_back(pkg);
			}
		}
		if (ImGui::Button("Load Asset Package")) {
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("pkg");
			if (fileName != "") {
				auto pkg = LoadPackage(fileName);
				soloPackages.push_back(pkg);
			}
		}
		ImGui::PopID();
	}
};