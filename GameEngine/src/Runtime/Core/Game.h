#pragma once

class Game {
private:
	bool isRunning;
	void SetupBaseSystems();
	void SetupLowLevelSystems();
	void LoadProject();
	void Setup();
	void Update();
	void Render();

public:
	Game();
	~Game();
	void Run();
};
