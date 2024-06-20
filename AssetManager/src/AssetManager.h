#pragma once
#include <vector>
#include <string>
#include <filesystem>

#include <imgui.h>

#include "FileDialog.h"
#include "Core/Log.h"
#include "Core/Guid.h"

#include "AssetPipline/AssetPackage.h"

const int FILE_PATH_SIZE = 41;

static int ResizeStringCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		auto str = (std::string*)(data->UserData);
		str->resize(data->BufTextLen);
	}
	return 0;
}



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
			auto fileName = ROSE_GETSYSTEM(FileDialog).SaveFile("pkg");
			if (fileName != "") {
				assetPackages.push_back(new AssetPackage());
			}
		}
		if (ImGui::Button("Open Asset Package")) {
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("pkg");
			if (fileName != "") {

				if (PackageIsLoaded(fileName)) {
					ROSE_LOG("Asset package already Open");
				}
				else {
					auto pkg = new AssetPackage();
					if (pkg->Load(fileName)) {
						assetPackages.push_back(pkg);
						ROSE_LOG("Asset package Opened");
					}
					else {
						delete pkg;
						ROSE_LOG("Failed to open asset package %s", fileName.c_str());
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
					auto fileName = ROSE_GETSYSTEM(FileDialog).SaveFile("pkg");
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
					auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("anim,lua,png,jpg");
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
						ROSE_LOG("Asset removed from Package");
					}
					else {
						ROSE_LOG("Asset not found in Package");
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
		asset->filePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), assetFileName);
	}
	bool IsAssetSelected() {
		return selectedAsset != nullptr;
	}
	void MetaDataEditor(AssetMetaData* metaData) {
		if (metaData->name.capacity() < FILE_PATH_SIZE) {
			metaData->name.reserve(FILE_PATH_SIZE);
		}
		ImGui::InputText("Name", &(metaData->name)[0], FILE_PATH_SIZE, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &metaData->name);

	}
	void TextureMetaDataEditor(TextureMetaData* metaData) {
		ImGui::InputInt("Pixels Per Unit", &metaData->ppu);
	}
	void RenderSelectedAsset() {
		ImGui::PushID(selectedAsset->guid);
		ImGui::LabelText("Guid", std::to_string(selectedAsset->guid).c_str());
		ImGui::LabelText("File Path", selectedAsset->filePath.c_str());
		if (ImGui::Button("Change Asset File")) {
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("anim,png,jpg,lua");
			if (fileName != "") {
				selectedAsset->filePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), fileName);
				if (Asset::GetAssetFileType(selectedAsset->filePath) != selectedAsset->assetType) {
					selectedAsset->assetType = Asset::GetAssetFileType(selectedAsset->filePath);
					switch (selectedAsset->assetType)
					{
					case AssetType::Texture:
						selectedAsset->metaData = new TextureMetaData();
						break;
					default:
						selectedAsset->metaData = new AssetMetaData();
						break;
					}
				}
			}
		}
		ImGui::SeparatorText(Asset::GetAssetTypeName(selectedAsset->assetType).c_str());
		MetaDataEditor(selectedAsset->metaData);
		switch (selectedAsset->assetType)
		{
		case AssetType::Texture:
			TextureMetaDataEditor((TextureMetaData*)selectedAsset->metaData);
			break;
		}
		ImGui::PopID();
	}
};