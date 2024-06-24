#pragma once
#include "BaseGame.h"

class Game: public BaseGame
{
private:
	bool isRunning;
	void Update();
	void Render();

public:
	Game();
	~Game();
	virtual void Run() override;
};
