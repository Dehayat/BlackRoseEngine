#pragma once
#include <vector>
#include <string>

#include <imgui.h>

#include "AssetStore/AssetPackage.h"

#include "Debug/Logger.h"

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
			assetPackages.push_back(new AssetPackage());
		}
		static char fileName[31] = "AssetPackage";
		if (ImGui::Button("Open Asset Package")) {
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
		ImGui::SameLine();
		ImGui::InputText(" ", fileName, 31);

		for (auto package : assetPackages) {
			if (ImGui::CollapsingHeader(package->filePath.c_str())) {
				ImGui::Indent();
				if (package->filePath.capacity() < 31) {
					package->filePath.reserve(31);
				}
				ImGui::InputText("File Path", &package->filePath[0], 31, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &package->filePath);
				if (ImGui::Button("Save")) {
					package->Save();
				}
				ImGui::SameLine();
				if (ImGui::Button("Close")) {
					if (selectedAsset != nullptr && package->ContainsAsset(selectedAsset)) {
						selectedAsset = nullptr;
					}
					assetPackages.erase(std::find(assetPackages.begin(), assetPackages.end(), package));
					delete package;
				}
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
				if (ImGui::Button("Add Texture Asset")) {
					package->AddAsset(AssetType::Texture);
					Logger::Log("New Texture Created");
				}
				if (ImGui::Button("Add Animation Asset")) {
					package->AddAsset(AssetType::Animation);
					Logger::Log("New Animation Created");
				}
				if (ImGui::BeginListBox("Assets")) {
					for (auto assetFile : package->assets) {
						bool selected = selectedAsset == assetFile;
						ImGui::Selectable((assetFile->metaData->name + " :" + std::to_string(assetFile->guid)).c_str(), &selected);

						if (selected) {
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
		}
	}
	bool IsAssetSelected() {
		return selectedAsset != nullptr;
	}
	void RenderSelectedAsset() {
		selectedAsset->Editor();
	}
};