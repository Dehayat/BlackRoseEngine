#pragma once
#include <vector>
#include <string>

#include <imgui.h>

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
			assetPackages.push_back(new AssetPackage());
		}
		static char fileName[31] = "assets/Packages/assets.pkg";
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
		ImGui::InputText("##OpenAssetPackageFileName", fileName, 31);

		for (auto package : assetPackages) {
			if (ImGui::CollapsingHeader(Label("Package", package->guid).c_str())) {
				ImGui::Indent();
				if (package->filePath.capacity() < 31) {
					package->filePath.reserve(31);
				}
				ImGui::InputText(Label("File Path", package->guid).c_str(), &package->filePath[0], 31, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &package->filePath);
				if (ImGui::Button(Label("Save", package->guid).c_str())) {
					package->Save();
				}
				ImGui::SameLine();
				if (ImGui::Button(Label("Close", package->guid).c_str())) {
					if (selectedAsset != nullptr && package->ContainsAsset(selectedAsset)) {
						selectedAsset = nullptr;
					}
					assetPackages.erase(std::find(assetPackages.begin(), assetPackages.end(), package));
					delete package;
				}
				if (ImGui::Button(Label("Remove Selected Asset", package->guid).c_str())) {
					if (IsAssetSelected() && package->ContainsAsset(selectedAsset)) {
						package->RemoveAsset(selectedAsset);
						selectedAsset = nullptr;
						Logger::Log("Asset removed from Package");
					}
					else {
						Logger::Log("Asset not found in Package");
					}
				}
				if (ImGui::Button(Label("Add Texture Asset", package->guid).c_str())) {
					package->AddAsset(AssetType::Texture);
					Logger::Log("New Texture Created");
				}
				if (ImGui::Button(Label("Add Animation Asset", package->guid).c_str())) {
					package->AddAsset(AssetType::Animation);
					Logger::Log("New Animation Created");
				}
				if (ImGui::Button(Label("Add Script Asset", package->guid).c_str())) {
					package->AddAsset(AssetType::Script);
					Logger::Log("New Script Created");
				}
				if (ImGui::BeginListBox(Label("Assets", package->guid).c_str())) {
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
		}
	}
	bool IsAssetSelected() {
		return selectedAsset != nullptr;
	}
	void RenderSelectedAsset() {
		selectedAsset->Editor();
	}
};