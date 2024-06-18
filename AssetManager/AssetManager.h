#pragma once
#include <vector>
#include <string>
#include <filesystem>

#include <imgui.h>

#include <FileDialog.h>

#include "AssetStore/AssetPackage.h"

#include "Debugging/Logger.h"

static std::string Label(const std::string& label, Guid guid) {
	return label + "##" + std::to_string(guid);
}

class AssetManager {

	std::vector<AssetPackage*> assetPackages;
	AssetFile* selectedAsset;

public:
	AssetManager() {
		selectedAsset = nullptr;
	}
	~AssetManager() {
	}

	bool PackageIsLoaded(const std::string& filePath) {
		for (auto pkg : assetPackages) {
			if (pkg->filePath == filePath) {
				return true;
			}
		}
		return false;
	}

	void Render()
	{
		if (ImGui::Button("New Asset Package")) {
			auto fileName = GETSYSTEM(FileDialog).SaveFile("pkg");
			if (fileName != "") {
				assetPackages.push_back(new AssetPackage());
			}
		}
		if (ImGui::Button("Open Asset Package")) {
			auto fileName = GETSYSTEM(FileDialog).OpenFile("pkg");
			if (fileName != "") {

				if (PackageIsLoaded(fileName)) {
					Logger::Log("Asset package already Open");
				}
				else {
					auto pkg = new AssetPackage();
					if (pkg->Load(fileName)) {
						assetPackages.push_back(pkg);
						Logger::Log("Asset package Opened");
					}
					else {
						delete pkg;
						Logger::Log("Failed to open asset package " + std::string(fileName));
					}
				}
			}
		}

		for (auto package : assetPackages) {
			ImGui::PushID(package->guid);
			if (ImGui::CollapsingHeader("Package")) {
				ImGui::Indent();
				if (package->filePath.capacity() < 31) {
					package->filePath.reserve(31);
				}
				if (ImGui::Button("Save")) {
					package->Save();
				}
				ImGui::SameLine();
				if (ImGui::Button("Save As")) {
					auto fileName = GETSYSTEM(FileDialog).SaveFile("pkg");
					if (fileName != "") {
						package->filePath = fileName;
						package->Save();
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Close")) {
					if (selectedAsset != nullptr && package->ContainsAsset(selectedAsset)) {
						selectedAsset = nullptr;
					}
					assetPackages.erase(std::find(assetPackages.begin(), assetPackages.end(), package));
					delete package;
				}
				if (ImGui::Button("Add Asset")) {
					auto fileName = GETSYSTEM(FileDialog).OpenFile("anim,lua,png,jpg");
					if (fileName != "") {
						AddAssetToPackage(package, fileName);
					}
				}
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4, 0.2, 0.2, 1));
				if (ImGui::Button("Remove Selected Asset")) {
					if (IsAssetSelected() && package->ContainsAsset(selectedAsset)) {
						package->RemoveAsset(selectedAsset);
						selectedAsset = nullptr;
						Logger::Log("Asset removed from Package");
					}
					else {
						Logger::Log("Asset not found in Package");
					}
				}
				ImGui::PopStyleColor();
				if (ImGui::BeginListBox("Assets")) {
					for (auto assetFile : package->assets) {
						bool selected = selectedAsset == assetFile;
						if (ImGui::Selectable((assetFile->metaData->name + " :" + std::to_string(assetFile->guid)).c_str(), &selected)) {
							selectedAsset = assetFile;
						}
					}
					ImGui::EndListBox();
					if (ImGui::IsItemClicked()) {
						selectedAsset = nullptr;
					}
				}
				ImGui::Unindent();
			}
			ImGui::PopID();
		}
	}
	void AddAssetToPackage(AssetPackage* package, const std::string assetFileName)
	{
		auto assetType = Asset::GetAssetFileType(assetFileName);
		auto asset = package->AddAsset(assetType);
		asset->filePath = GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), assetFileName);
	}
	bool IsAssetSelected() {
		return selectedAsset != nullptr;
	}
	void RenderSelectedAsset() {
		selectedAsset->Editor();
	}
};