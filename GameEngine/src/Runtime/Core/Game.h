#pragma once

class Game {
private:
	bool isRunning;
	void SetupBaseSystems();
	void SetupLowLevelSystems();
	void LoadAssets();
	void LoadLevel();
	void Setup();
	void Update();
	void Render();

public:
	Game();
	~Game();
	void Run();
};
