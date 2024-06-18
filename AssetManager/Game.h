#pragma once

class Game {
private:
	bool isRunning;

public:
	Game();
	~Game();
	void SetupBaseSystems();
	void Run();
	void Update();
	void Render();
};
