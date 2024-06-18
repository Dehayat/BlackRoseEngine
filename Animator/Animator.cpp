#include "Animator.h"

#include <filesystem>

#include <FileDialog.h>

#include "AssetStore/AssetStore.h"


Animator::Animator() :maxDt(0.0166) {
	selectedAsset = nullptr;
	animationHandle = AssetHandle();
	player = AnimationComponent("animationFile");
	animationDuration = 1;
	currentAnimationTime = 0;
	isPlaying = false;
	selectedFrame = -1;
	scrollToSelected = false;
	selectedEvent = -1;
	scrollToEvent = false;
	lastTime = 0;
	selectedAnimation = nullptr;
}

AssetPackage* Animator::GetPackage(const std::string& filePath) {
	for (auto pkg : assetPackages) {
		if (pkg->filePath == filePath) {
			return pkg;
		}
	}
	return nullptr;
}

void Animator::Render()
{
	int w, h;
	auto window = GETSYSTEM(SdlContainer).GetWindow();
	SDL_GetWindowSize(window, &w, &h);
	ImGui::Begin("Package Loader", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(w / 4, h));
	ImGui::SetWindowPos(ImVec2(0, 0));
	PackageLoaderEditor();
	ImGui::End();

	ImGui::Begin("Animation Asset", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowSize(ImVec2(w / 4, h));
	ImGui::SetWindowPos(ImVec2(3 * w / 4, 0));
	AnimationAssetEditor(ImVec2(w / 4, h));
	ImGui::End();

	ImGui::Begin("Animation Player", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowSize(ImVec2(w / 2, h / 3 + 50));
	ImGui::SetWindowPos(ImVec2(w / 4, h - h / 3 - 50));
	AnimationPlayerEditor(ImVec2(w / 2, h / 3 + 50));
	ImGui::End();

	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowSize(ImVec2(w / 2, 2 * h / 3 - 50));
	ImGui::SetWindowPos(ImVec2(w / 4, 0));
	AnimationViewportEditor(ImVec2(w / 2, 2 * h / 3 - 50));
	ImGui::End();
}

void Animator::PackageLoaderEditor()
{
	if (ImGui::Button("Load Asset Package")) {
		auto fileName = OpenFile("pkg");

		if (fileName != "") {
			auto pkg = GetPackage(fileName);
			if (pkg != nullptr) {
				Logger::Log("Reloading Asset Package");
				assetPackages.erase(std::find(assetPackages.begin(), assetPackages.end(), pkg));
				delete pkg;
			}
			pkg = new AssetPackage();
			if (pkg->Load(fileName)) {
				assetPackages.push_back(pkg);
				GETSYSTEM(AssetStore).LoadPackage(fileName);
				Logger::Log("Asset package Opened");
			}
			else {
				delete pkg;
				Logger::Log("Failed to open asset package " + std::string(fileName));
			}
		}
	}

	//ImGui::SameLine();

	for (auto package : assetPackages) {
		if (ImGui::CollapsingHeader(Label("Package", package->guid).c_str())) {
			ImGui::Indent();
			if (ImGui::Button(Label("Close", package->guid).c_str())) {
				if (selectedAsset != nullptr && package->ContainsAsset(selectedAsset)) {
					selectedAsset = nullptr;
				}
				assetPackages.erase(std::find(assetPackages.begin(), assetPackages.end(), package));
				delete package;
				continue;
			}
			ImGui::SeparatorText("Animations");
			if (ImGui::BeginListBox(Label("Animations", package->guid).c_str())) {
				for (auto assetFile : package->assets) {
					if (assetFile->assetType != AssetType::Animation) {
						continue;
					}
					bool selected = selectedAnimation == assetFile;
					if (ImGui::Selectable((assetFile->metaData->name + " :" + std::to_string(assetFile->guid)).c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick)) {
						selectedAnimation = assetFile;
						if (ImGui::IsMouseDoubleClicked(0)) {
							loadSelectedAnimaiton = true;
						}
					}
				}
				ImGui::EndListBox();
			}
			ImGui::SeparatorText("Textures");
			if (ImGui::BeginListBox(Label("Textures", package->guid).c_str())) {
				for (auto assetFile : package->assets) {
					if (assetFile->assetType != AssetType::Texture) {
						continue;
					}
					bool selected = selectedAsset == assetFile;
					if (ImGui::Selectable((assetFile->metaData->name + " :" + std::to_string(assetFile->guid)).c_str(), &selected, ImGuiSelectableFlags_AllowDoubleClick)) {
						selectedAsset = assetFile;
						if (ImGui::IsMouseDoubleClicked(0)) {
							auto animation = (Animation*)animationHandle.asset;
							if (animation != nullptr) {
								animation->texture = selectedAsset->metaData->name;
							}
						}
					}
				}
				ImGui::EndListBox();
			}
			ImGui::Unindent();
		}
	}
	if (selectedAsset != nullptr) {
		auto animation = (Animation*)animationHandle.asset;
		if (animation != nullptr) {
			auto windowWidth = ImGui::GetWindowSize().x;
			auto windowHeight = ImGui::GetWindowSize().y;
			auto buttonSize = ImGui::CalcTextSize("Use as Sprite Atlas");
			ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
			if (ImGui::Button("Set Texture as Sprite Atlas")) {
				animation->texture = selectedAsset->metaData->name;
			}
		}
		if (ImGui::BeginChild("Preview", ImVec2(280, 280), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
			auto asset = (TextureAsset*)(GETSYSTEM(AssetStore).GetAsset(selectedAsset->metaData->name).asset);
			auto windowWidth = ImGui::GetWindowSize().x;
			auto windowHeight = ImGui::GetWindowSize().y;

			ImGui::SetCursorPosX((windowWidth - 260) * 0.5f);
			ImGui::SetCursorPosY((windowHeight - 260) * 0.5f);
			ImGui::Image(asset->texture, ImVec2(260, 260), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0.5, 0.5, 0.5, 0.8));
		}
		ImGui::EndChild();
	}
}

void Animator::AnimationAssetEditor(ImVec2 size)
{
	if (ImGui::Button("Create new Animation File")) {
		auto saveFileName = SaveFile("anim");
		if (saveFileName != "") {
			animationFile = saveFileName;
			animationHandle = GETSYSTEM(AssetStore).NewAnimation("animationFile");
		}
	}

	if (ImGui::Button("Open")) {
		auto openFileName = OpenFile("anim");
		if (openFileName != "") {
			animationFile = openFileName;
			GETSYSTEM(AssetStore).LoadAnimation("animationFile", animationFile);
			animationHandle = GETSYSTEM(AssetStore).GetAsset("animationFile");
		}
	}
	if (loadSelectedAnimaiton) {
		if (loadSelectedAnimaiton) {
			loadSelectedAnimaiton = false;
		}
		animationFile = selectedAnimation->filePath;
		GETSYSTEM(AssetStore).LoadAnimation("animationFile", animationFile);
		animationHandle = GETSYSTEM(AssetStore).GetAsset("animationFile");
	}
	if (animationHandle.asset != nullptr)
	{
		ImGui::SeparatorText("Animation File");
		ImGui::Text(animationFile.c_str());
		if (ImGui::Button("Save")) {
			GETSYSTEM(AssetStore).SaveAnimation("animationFile", animationFile);
		}
		auto animation = (Animation*)animationHandle.asset;
		if (animation->texture == "") {
			ImGui::LabelText("Sprite atlas", "-Select from package-");
		}
		else {
			ImGui::LabelText("Sprite atlas", animation->texture.c_str());
		}
		ImGui::InputInt2("Sprite size##spriteSizeInput", &animation->spriteFrameWidth);
		ImGui::Checkbox("Loop", &animation->isLooping);
		static float frameDuration = 0.1f;
		if (ImGui::CollapsingHeader("Generate Frames from Atlas")) {
			ImGui::Indent();
			ImGui::PushItemWidth(100);
			ImGui::SliderFloat("Frame duration (s)", &frameDuration, 0, 5);
			if (ImGui::Button("Generate", ImVec2(size.x - 50, 0))) {
				GenerateFramesFromAtlas(frameDuration);
			}
			ImGui::Unindent();
		}

		int i = 0;
		int addFrame = -1;
		int deleteFrame = -1;
		ImGui::SeparatorText("Frames");
		for (auto frame : animation->frames) {
			if (RenderFrame(frame, i) == -1) {
				deleteFrame = i;
			}
			i++;
			auto cursor = ImGui::GetCursorPos();
			if (ImGui::Button(("+##" + std::to_string(frame->id)).c_str(), ImVec2(size.x, 20))) {
				addFrame = i;
				selectedFrame = -1;
			}
		}
		scrollToSelected = false;
		if (addFrame != -1) {
			auto animation = (Animation*)animationHandle.asset;
			animation->frames.insert(animation->frames.begin() + addFrame, new Frame());
		}
		if (deleteFrame != -1) {
			auto animation = (Animation*)animationHandle.asset;
			animation->frames.erase(animation->frames.begin() + deleteFrame);
		}

	}
}
int Animator::RenderFrame(Frame* frame, int index)
{
	bool del = false;
	if (selectedFrame == index) {
		if (scrollToSelected) {
			ImGui::SetScrollHereY();
		}
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.7, 0.7, 0.3, 1));
	}
	else {
		auto color = ImGui::GetStyleColorVec4(ImGuiCol_Border);
		ImGui::PushStyleColor(ImGuiCol_Border, color);
	}
	if (ImGui::BeginChild(("Frame##" + std::to_string(frame->id)).c_str(), ImVec2(0, 60), true, ImGuiWindowFlags_NoCollapse)) {
		ImGui::Text("Frame");
		ImGui::PushItemWidth(50);
		ImGui::DragInt(("offset##" + std::to_string(frame->id)).c_str(), &frame->framePosition, 1, 0, GetFrameCount() - 1);
		ImGui::PushItemWidth(50);
		ImGui::SameLine();
		ImGui::DragFloat(("(s)##" + std::to_string(frame->id)).c_str(), &frame->frameDuration, 0.05, 0, 200);
		ImGui::PushItemWidth(100);
		ImGui::SameLine();
		auto size = ImGui::GetContentRegionAvail();
		auto pos = ImGui::GetCursorPos();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.2, 0.2, 1));
		ImGui::SetCursorPos(ImVec2(pos.x + size.x - 20, pos.y - 18));
		if (ImGui::Button(("-##" + std::to_string(frame->id)).c_str(), ImVec2(20, 45)))
		{
			selectedFrame = -1;
			del = true;
		}
		ImGui::PopStyleColor();
	}
	ImGui::EndChild();
	if (ImGui::IsItemClicked()) {
		selectedFrame = index;
	}
	ImGui::PopStyleColor();
	if (del) {
		return -1;
	}
	return 0;
}


void Animator::AnimationPlayerEditor(ImVec2 size)
{
	{
		ImGuiStyle& style = ImGui::GetStyle();
		float width = 0.0f;
		width += ImGui::CalcTextSize("<<").x;
		width += ImGui::CalcTextSize("<").x;
		width += ImGui::CalcTextSize("O").x;
		width += ImGui::CalcTextSize("|>").x;
		width += ImGui::CalcTextSize(">").x;
		width += ImGui::CalcTextSize(">>").x;
		width += style.ItemSpacing.x * 10;
		float off = (size.x - width) * 0.5;
		ImGui::SetCursorPosX(off);
		if (ImGui::Button("<<")) {
			currentAnimationTime = 0;
			player.SetTime(0);
			isPlaying = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("<")) {
			isPlaying = false;
			auto animation = (Animation*)animationHandle.asset;
			if (animation != nullptr && (player.currentFrame > 0 ||
				(currentAnimationTime >= animationDuration && animation->frames.size() > 1)
				))
			{
				int f = player.currentFrame - 1;
				if (currentAnimationTime >= animationDuration) {
					f = animation->frames.size() - 2;
				}
				float t = 0;
				for (int i = 0; i < f; i++) {
					t += animation->frames[i]->frameDuration;
				}
				currentAnimationTime = t;
				player.SetTime(currentAnimationTime);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("O")) {
			currentAnimationTime = 0;
			player.SetTime(0);
			isPlaying = false;
		}
		ImGui::SameLine();
		if (!isPlaying) {
			if (ImGui::Button("|>")) {
				if (currentAnimationTime >= animationDuration) {
					currentAnimationTime = 0;
					player.Reset();
				}
				isPlaying = true;
				lastTime = SDL_GetTicks64();
			}
		}
		else {
			if (ImGui::Button("||")) {
				isPlaying = false;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(">")) {
			isPlaying = false;
			auto animation = (Animation*)animationHandle.asset;
			if (animation != nullptr && (player.currentFrame < animation->frames.size() - 1 && currentAnimationTime < animationDuration))
			{
				int f = player.currentFrame + 1;
				float t = 0;
				for (int i = 0; i < f; i++) {
					t += animation->frames[i]->frameDuration;
				}
				currentAnimationTime = t;
				player.SetTime(currentAnimationTime);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(">>")) {
			isPlaying = false;
			auto animation = (Animation*)animationHandle.asset;
			if (animation != nullptr) {
				float t = animationDuration - animation->frames[animation->frames.size() - 1]->frameDuration;
				currentAnimationTime = t;
				player.SetTime(currentAnimationTime);
			}
		}
	}
	{
		animationDuration = player.GetAnimationDuration();
		auto padding = ImGui::GetStyle().WindowPadding;
		ImGui::PushItemWidth(size.x);
		if (ImGui::SliderFloat("##Timeline", &currentAnimationTime, 0, animationDuration, "%0.3fs")) {
			player.SetTime(currentAnimationTime);
		}
		if (ImGui::IsItemClicked(0) && ImGui::IsMouseDoubleClicked(0)) {
			auto animation = (Animation*)animationHandle.asset;
			if (animation != nullptr) {
				animation->AddEvent(new AnimationEventData(currentAnimationTime));
				selectedEvent = animation->animationEvents.size() - 1;
				scrollToEvent = true;
			}
		}
	}

	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		if (animationHandle.asset != nullptr)
		{
			auto animation = (Animation*)animationHandle.asset;
			int i = 0;
			for (auto frame : animation->frames) {
				RenderFrameImage(frame, i, size.x);
				if (i + 1 < animation->frames.size()) {
					ImGui::SameLine();
				}
				i++;
			}
		}
		ImGui::PopStyleVar(6);
	}

	auto animation = (Animation*)animationHandle.asset;
	if (animation != nullptr) {
		int deleteEvent = -1;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		if (ImGui::Button("Add Event")) {
			animation->AddEvent(new AnimationEventData());
			selectedEvent = animation->animationEvents.size() - 1;
			scrollToEvent = true;
		}
		if (ImGui::BeginChild("Events"))
		{
			if (ImGui::BeginTable("EventsTable", 2)) {
				ImGui::TableNextRow();
				int i = 0;
				for (auto eventData : animation->animationEvents) {
					ImGui::TableNextColumn();
					if (RenderEvent(eventData, i) == -1) {
						deleteEvent = i;
					}
					i++;
				}
				ImGui::EndTable();
			}
		}
		ImGui::EndChild();


		if (deleteEvent != -1) {
			selectedEvent = -1;
			animation->animationEvents.erase(animation->animationEvents.begin() + deleteEvent);
		}
	}
	scrollToEvent = false;
}

int Animator::RenderEvent(AnimationEventData* eventData, int index)
{
	bool del = false;
	if (selectedEvent == index) {
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.7, 0.7, 0.3, 1));
	}
	else {
		auto color = ImGui::GetStyleColorVec4(ImGuiCol_Border);
		ImGui::PushStyleColor(ImGuiCol_Border, color);
	}

	if (ImGui::BeginChild(("Event##" + std::to_string(eventData->id)).c_str(), ImVec2(0, 60), true, ImGuiWindowFlags_NoCollapse)) {
		auto width = ImGui::GetContentRegionAvail().x;
		if (eventData->eventName.capacity() < 31) {
			eventData->eventName.reserve(31);
		}
		ImGui::Text("Event");
		ImGui::SameLine();
		ImGui::PushItemWidth(100);
		ImGui::InputText(("##" + std::to_string(eventData->id)).c_str(), &eventData->eventName[0], 31, ImGuiInputTextFlags_CallbackResize, ResizeStringCallback, &eventData->eventName);
		ImGui::SameLine();
		if (ImGui::Button(("Align##" + std::to_string(eventData->id)).c_str(), ImVec2(50, 0))) {
			eventData->eventTime = currentAnimationTime;
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(width - 22);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.2, 0.2, 1));
		if (ImGui::Button(("-##" + std::to_string(eventData->id)).c_str(), ImVec2(30, 0))) {
			del = true;
		}
		ImGui::PopStyleColor();
		ImGui::PushItemWidth(width);
		ImGui::SliderFloat(("##EventSlider" + std::to_string(eventData->id)).c_str(), &eventData->eventTime, 0, animationDuration, "%0.3fs");

	}
	ImGui::PopStyleColor();
	ImGui::EndChild();
	if (selectedEvent == index) {
		if (scrollToEvent) {
			ImGui::SetScrollHereY();
		}
	}
	if (del) {
		return -1;
	}
	return 0;
}

void Animator::RenderFrameImage(Frame* frame, int id, float fullWidth)
{
	fullWidth -= 20;
	auto animation = (Animation*)animationHandle.asset;
	auto sourceRect = SDL_Rect(animation->GetSourceRect(id));
	auto texture = (TextureAsset*)GETSYSTEM(AssetStore).GetAsset(animation->texture).asset;
	int texW, texH;
	if (texture == nullptr) {
		ImGui::Button("Sprite Not Found", ImVec2(fullWidth * (frame->frameDuration / animationDuration), 100));
		return;
	}
	SDL_QueryTexture(texture->texture, nullptr, nullptr, &texW, &texH);
	auto uv0 = ImVec2((float)sourceRect.x / texW, (float)sourceRect.y / texH);
	auto uv1 = ImVec2(((float)sourceRect.x + sourceRect.w) / texW, ((float)sourceRect.y + sourceRect.h) / texH);
	auto borderColor = ImVec4(0.5, 0.5, 0.5, 1);
	if (selectedFrame == id) {
		borderColor = ImVec4(0.7, 0.7, 0.3, 1);
	}
	if (texture != nullptr && texture->texture != nullptr) {
		auto windowWidth = ImGui::GetWindowSize().x;
		auto windowHeight = ImGui::GetWindowSize().y;
		ImGui::Image(texture->texture, ImVec2(fullWidth * (frame->frameDuration / animationDuration), 100), uv0, uv1, ImVec4(1, 1, 1, 1), borderColor);
	}
	else {
		ImGui::Button("Sprite Not Found", ImVec2(fullWidth * (frame->frameDuration / animationDuration), 100));
	}
	if (ImGui::IsItemClicked()) {
		selectedFrame = id;
		scrollToSelected = true;
	}
}

void Animator::AnimationViewportEditor(ImVec2 size)
{
	auto animation = (Animation*)animationHandle.asset;
	if (animation != nullptr) {
		auto sourceRect = SDL_Rect(animation->GetSourceRect(player.currentFrame));
		auto texture = (TextureAsset*)GETSYSTEM(AssetStore).GetAsset(animation->texture).asset;
		if (texture != nullptr) {
			if (isPlaying) {

				auto dt = (SDL_GetTicks64() - lastTime) / 1000.0f;
				if (dt > maxDt) {
					dt = maxDt;
				}
				lastTime = SDL_GetTicks64();

				currentAnimationTime += dt;
				player.Update(dt);
				if (player.isOver) {
					player.isOver = false;
					isPlaying = false;
					currentAnimationTime = animationDuration;
				}
				else {
					if (player.justFinished) {
						currentAnimationTime = 0;
					}
				}
			}
			int texW, texH;
			SDL_QueryTexture(texture->texture, nullptr, nullptr, &texW, &texH);
			auto uv0 = ImVec2((float)sourceRect.x / texW, (float)sourceRect.y / texH);
			auto uv1 = ImVec2(((float)sourceRect.x + sourceRect.w) / texW, ((float)sourceRect.y + sourceRect.h) / texH);
			if (texture != nullptr && texture->texture != nullptr) {
				auto windowWidth = ImGui::GetWindowSize().x;
				auto windowHeight = ImGui::GetWindowSize().y;

				auto maxImgSize = 370;

				auto imgHeight = maxImgSize;
				auto imgWidth = imgHeight * animation->spriteFrameWidth / texH;
				if (imgWidth > maxImgSize) {
					imgWidth = maxImgSize;
					imgHeight = imgWidth * texH / animation->spriteFrameWidth;
				}

				ImGui::SetCursorPosX((windowWidth - imgWidth) * 0.5f);
				ImGui::SetCursorPosY((windowHeight - imgHeight) * 0.5f);
				ImGui::Image(texture->texture, ImVec2(imgWidth, imgHeight), uv0, uv1, ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));
			}
			else {
				ImGui::Text("sprite atlas texture not loaded");
			}
		}
		else {
			ImGui::Text("sprite atlas texture not loaded");
		}
	}
	else {
		ImGui::Text("No Animation Loaded");
	}
}

bool Animator::IsAssetSelected() {
	return selectedAsset != nullptr;
}

void Animator::GenerateFramesFromAtlas(float frameDuration)
{
	auto animation = (Animation*)animationHandle.asset;
	auto texture = (TextureAsset*)GETSYSTEM(AssetStore).GetAsset(animation->texture).asset;
	if (animation != nullptr && texture != nullptr) {
		int texW, texH;
		SDL_QueryTexture(texture->texture, nullptr, nullptr, &texW, &texH);
		animation->frames.clear();
		int posX = 0;
		int i = 0;
		while (posX + animation->spriteFrameWidth <= texW) {
			animation->AddFrame(new Frame(i, frameDuration));
			posX += animation->spriteFrameWidth;
			i++;
		}
	}
}

void Animator::RenderSelectedAsset() {
	selectedAsset->Editor();
}

int Animator::GetFrameCount() {
	auto animation = (Animation*)animationHandle.asset;
	auto texture = (TextureAsset*)GETSYSTEM(AssetStore).GetAsset(animation->texture).asset;
	if (texture != nullptr && texture->texture != nullptr) {
		int texW, texH;
		SDL_QueryTexture(texture->texture, nullptr, nullptr, &texW, &texH);
		return texW / animation->spriteFrameWidth;
	}
	return 50;
}
std::string Animator::SaveFile(const std::string& extension)
{
	return GETSYSTEM(FileDialog).SaveFile(extension);
}

std::string Animator::OpenFile(const std::string& extension)
{
	return GETSYSTEM(FileDialog).OpenFile(extension);
}
