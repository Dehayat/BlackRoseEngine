#pragma once
#include <vector>
#include <string>
#include <filesystem>

#include <imgui.h>

#include "ImguiHelper.h"

#include "FileDialog.h"
#include "Core/Log.h"
#include "Core/Guid.h"

#include "AssetPipline/AssetPackage.h"

const int FILE_PATH_SIZE = 40;

static std::string Label(const std::string& label, Guid guid)
{
	return label + "##" + std::to_string(guid);
}

class AssetManager
{

	AssetFile* selectedAsset;

public:
	AssetManager()
	{
		ROSE_CREATESYSTEM(ProjectManager);
		selectedAsset = nullptr;
	}
	~AssetManager()
	{
	}

	void Render()
	{
		auto package = ROSE_GETSYSTEM(ProjectManager).GetCurrentPackage();
		if (package == nullptr) {
			return;
		}
		ImGui::PushID(package->guid);

		ImGui::SeparatorText(package->filePath.c_str());
		if (ImGui::BeginListBox("Assets", ImVec2(0, 500))) {
			for (auto assetFile : package->assets) {
				bool selected = selectedAsset == assetFile;
				if (ImGui::Selectable((assetFile->metaData->name + " :" + std::to_string(assetFile->guid)).c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick)) {
					selectedAsset = assetFile;
				}
			}
			ImGui::EndListBox();
		}


		if (package->filePath.capacity() < 31) {
			package->filePath.reserve(31);
		}
		if (ImGui::Button("Save")) {
			package->Save();
		}
		//TODO: fix save as button
		//ImGui::SameLine();
		//if (ImGui::Button("Save As")) {
		//	auto fileName = ROSE_GETSYSTEM(FileDialog).SaveFile("pkg");
		//	if (fileName != "") {
		//		package->filePath = fileName;
		//		package->Save();
		//	}
		//}
		if (ImGui::Button("Add Asset"))
		{
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("anim,lua,png,jpg");
			if (fileName != "") {
				AddAssetToPackage(package, fileName);
			}
		}
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4, 0.2, 0.2, 1));
		if (ImGui::Button("Remove Selected Asset")) {
			if (IsAssetSelected() && package->ContainsAsset(selectedAsset))
			{
				package->RemoveAsset(selectedAsset);
				selectedAsset = nullptr;
				ROSE_LOG("Asset removed from Package");
			}
			else
			{
				ROSE_LOG("Asset not found in Package");
			}
		}
		ImGui::PopStyleColor();
		ImGui::PopID();
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
	void MetaDataEditor(AssetMetaData* metaData)
	{
		if (metaData->name.capacity() < FILE_PATH_SIZE)
		{
			metaData->name.reserve(FILE_PATH_SIZE);
		}
		Imgui_InputText("Name", metaData->name, FILE_PATH_SIZE);

	}
	void TextureMetaDataEditor(TextureMetaData* metaData)
	{
		ImGui::InputInt("Pixels Per Unit", &metaData->ppu);
	}
	void RenderSelectedAsset()
	{
		ImGui::PushID(selectedAsset->guid);
		ImGui::LabelText("Guid", std::to_string(selectedAsset->guid).c_str());
		ImGui::LabelText("File Path", selectedAsset->filePath.c_str());
		if (ImGui::Button("Change Asset File"))
		{
			auto fileName = ROSE_GETSYSTEM(FileDialog).OpenFile("anim,png,jpg,lua");
			if (fileName != "")
			{
				selectedAsset->filePath = ROSE_GETSYSTEM(FileDialog).GetRelativePath(std::filesystem::current_path().string(), fileName);
				if (Asset::GetAssetFileType(selectedAsset->filePath) != selectedAsset->assetType)
				{
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