#pragma once
#include "Application/Application.h"

class BaseGame: public IApplication
{
protected:
	void SetupBaseSystems();
	void SetupLowLevelSystems();
	void LoadProject();
	void Setup();

public:
	BaseGame();
	~BaseGame();
};
