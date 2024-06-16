#pragma once
#include <string>

#include <AssetStore/AssetHandle.h>

struct ScriptAsset : Asset {
	std::string script;
	ScriptAsset(const std::string& script) :script(script) {}
};
