#pragma once
#include <vector>
#include <string>
#include <map>

#include <imgui.h>
#include <ryml/ryml.hpp>

#include "SdlContainer.h"

#include "Entity.h"

#include "AssetStore/AssetHandle.h"
#include "AssetStore/AnimationAsset.h"
#include "AssetStore/TextureAsset.h"

#include "Systems.h"
#include "FileResource.h"

#include "Debug/Logger.h"

struct AssetMetaData {
	virtual void Editor() {}
	virtual void Serialize(ryml::NodeRef& parent) {}
};

struct TextureMetaData :AssetMetaData {
	int ppu;
	void Editor() {
		ImGui::InputInt("Pixels Per Unit", &ppu);
	}
	void Serialize(ryml::NodeRef& node) {
		node["ppu"] << ppu;
	}
	TextureMetaData() {
		ppu = 100;
	}
	TextureMetaData(ryml::NodeRef& node) {
		node["ppu"] >> ppu;
	}
};

struct AnimationMetaData :AssetMetaData {
	void Editor() {
	}
};
static int ResizeStringCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		auto str = (std::string*)(data->UserData);
		str->resize(data->BufTextLen);
	}
	return 0;
}

struct AssetFile {
	Guid guid;
	AssetFile(AssetType assetType = AssetType::Empty) {
		guid = GETSYSTEM(Entities).GenerateGuid();
		filePath = "";
		this->assetType = assetType;
		switch (assetType)
		{
		case AssetType::Texture:
			metaData = new TextureMetaData();
			break;
		case AssetType::Animation:
			metaData = new AnimationMetaData();
			break;
		default:
			metaData = new AssetMetaData();
			break;
		}
	}
	~AssetFile() {
		if (metaData != nullptr) {
			delete metaData;
		}
	}
	AssetFile(ryml::NodeRef& node) {
		node |= ryml::MAP;
		node["Guid"] >> guid;
		int assetTypeTemp;
		node["AssetType"] >> assetTypeTemp;
		this->assetType = (AssetType)assetTypeTemp;
		node["FilePath"] >> filePath;
		switch (assetType)
		{
		case AssetType::Texture:
			metaData = new TextureMetaData(node);
			break;
		case AssetType::Animation:
			metaData = new AnimationMetaData();
			break;
		default:
			metaData = new AssetMetaData();
			break;
		}
	}
	void Editor() {
		ImGui::Text(std::to_string(guid).c_str());
		if (filePath.capacity() < 21) {
			filePath.reserve(21);
		}
		ImGui::InputText("File Path", &filePath[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &filePath);
		if (metaData != nullptr) {
			metaData->Editor();
		}
	}
	void Serialize(ryml::NodeRef& parent) {
		auto node = parent.append_child();
		node |= ryml::MAP;
		node["Guid"] << guid;
		node["AssetType"] << (int)assetType;
		node["FilePath"] << filePath;
		metaData->Serialize(node);
	}
	std::string filePath;
	AssetMetaData* metaData;
	AssetType assetType;
};

struct AssetPackage {
	std::vector<AssetFile*> assets;
	std::string filePath;
	Guid guid;
	AssetPackage() {
		guid = GETSYSTEM(Entities).GenerateGuid();
		filePath = "";
	}
	~AssetPackage() {
		for (auto asset : assets) {
			if (asset != nullptr) {
				delete asset;
			}
		}
	}
	AssetFile* AddAsset(AssetType assetType) {
		AssetFile* assetFile = new AssetFile(assetType);
		assets.push_back(assetFile);
		return assetFile;
	}
	void RemoveAsset(AssetFile* assetFile) {
		if (assetFile != nullptr) {
			assets.erase(std::find(assets.begin(), assets.end(), assetFile));
			delete assetFile;
		}
	}
	bool ContainsAsset(AssetFile* assetFile) {
		return std::find(assets.begin(), assets.end(), assetFile) != assets.end();
	}
	void Save() {
		Logger::Log("Saving asset package");
		auto fileHandle = FileResource(filePath, "w+");
		if (fileHandle.file == nullptr) {
			Logger::Error("Couldnt create file " + filePath);
			return;
		}
		auto tree = ryml::Tree();
		auto root = tree.rootref();
		Serialize(root);
		std::string buffer = ryml::emitrs_yaml<std::string>(tree);
		SDL_RWwrite(fileHandle.file, buffer.data(), 1, buffer.size());
	}
	void Serialize(ryml::NodeRef& node) {
		node |= ryml::MAP;
		node["Guid"] << guid;
		auto assetsNode = node.append_child();
		assetsNode.set_key("Assets");
		assetsNode |= ryml::SEQ;
		for (auto asset : assets) {
			asset->Serialize(assetsNode);
		}
	}
	bool Load(const std::string& filePath) {
		auto fileHandle = FileResource(filePath);
		if (fileHandle.file == nullptr) {
			Logger::Error("File" + filePath + " not found");
			return false;
		}
		std::string fileString = std::string("\0", SDL_RWsize(fileHandle.file));
		SDL_RWread(fileHandle.file, &fileString[0], sizeof(fileString[0]), fileString.size());
		auto tree = ryml::parse_in_arena(ryml::to_csubstr(fileString));
		auto root = tree.rootref();
		Deserialize(root);
		this->filePath = filePath;
		return true;
	}

	void Deserialize(ryml::NodeRef& node)
	{
		node |= ryml::MAP;
		node["Guid"] >> guid;
		auto assets = node.find_child("Assets");
		auto child = assets.first_child();
		for (int i = 0; i < assets.num_children(); i++) {
			this->assets.push_back(new AssetFile(child));
			child = child.next_sibling();
		}
	}
};

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
		static char fileName[20] = "AssetPackage";
		if (ImGui::Button("Load Asset Package")) {
			if (PackageIsLoaded(fileName)) {
				Logger::Log("Asset package already loaded");
			}
			else {
				auto pkg = new AssetPackage();
				if (pkg->Load(fileName)) {
					assetPackages.push_back(pkg);
					Logger::Log("Asset package Loaded");
				}
				else {
					delete pkg;
					Logger::Log("Failed to load asset package");
				}
			}

		}
		ImGui::SameLine();
		ImGui::InputText(" ", fileName, 20);

		for (auto package : assetPackages) {
			if (ImGui::CollapsingHeader(std::to_string(package->guid).c_str())) {
				ImGui::Indent();
				if (package->filePath.capacity() < 21) {
					package->filePath.reserve(21);
				}
				ImGui::InputText("File Path", &package->filePath[0], 21, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &package->filePath);
				if (ImGui::Button("Save")) {
					package->Save();
				}
				ImGui::SameLine();
				if (ImGui::Button("Unload")) {
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
						ImGui::Selectable(std::to_string(assetFile->guid).c_str(), &selected);

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