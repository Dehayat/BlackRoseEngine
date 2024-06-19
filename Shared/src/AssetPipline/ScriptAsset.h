#pragma once
#include <string>

#include "Asset.h"

struct ScriptAsset : Asset {
	std::string script;
	ScriptAsset(const std::string& script);
};
