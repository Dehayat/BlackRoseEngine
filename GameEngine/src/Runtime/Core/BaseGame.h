#pragma once
#include <string>

#include "Application/Application.h"

class BaseGame: public IApplication
{
protected:
	void SetupBaseSystems();
	void SetupLowLevelSystems();
	void LoadProject(const std::string& projectName);
	void Setup();

public:
	BaseGame();
	~BaseGame();
};
