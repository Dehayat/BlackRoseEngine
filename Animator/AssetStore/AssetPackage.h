#pragma once
#include <vector>
#include <string>

#include <imgui.h>
#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>

#include "../Entity.h"

#include "AssetHandle.h"
#include "AnimationAsset.h"
#include "TextureAsset.h"

#include "../Systems.h"

#include "../FileResource.h"

#include "../Debugging/Logger.h"

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

struct AssetMetaData {
	std::string name;
	virtual void Editor() {
		if (name.capacity() < FILE_PATH_SIZE) {
			name.reserve(FILE_PATH_SIZE);
		}
		ImGui::InputText("Name", &name[0], FILE_PATH_SIZE, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &name);
	}
	virtual void Serialize(ryml::NodeRef& node)
	{
		node["name"] << name;
	}
	AssetMetaData() {
		name = "";
	}
	AssetMetaData(ryml::NodeRef& node) {
		node["name"] >> name;
	}
};

struct TextureMetaData :AssetMetaData {
	int ppu;
	void Editor() {
		AssetMetaData::Editor();
		ImGui::InputInt("Pixels Per Unit", &ppu);
	}
	void Serialize(ryml::NodeRef& node) {
		AssetMetaData::Serialize(node);
		node["ppu"] << ppu;
	}
	TextureMetaData() {
		name = "";
		ppu = 100;
	}
	TextureMetaData(ryml::NodeRef& node) :AssetMetaData(node) {
		node["ppu"] >> ppu;
	}
};

struct AnimationMetaData :AssetMetaData {
	AnimationMetaData() {}
	AnimationMetaData(ryml::NodeRef& node) :AssetMetaData(node) {
	}
};

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
			metaData = new AnimationMetaData(node);
			break;
		default:
			metaData = new AssetMetaData(node);
			break;
		}
	}
	void Editor() {
		ImGui::Text(std::to_string(guid).c_str());
		if (filePath.capacity() < FILE_PATH_SIZE) {
			filePath.reserve(FILE_PATH_SIZE);
		}
		ImGui::InputText("File Path", &filePath[0], FILE_PATH_SIZE, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &filePath);
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