#pragma once

class Game {
private:
	bool isRunning;


public:
	Game();
	~Game();
	void SetupBaseSystems();
	void SetupLowLevelSystems();
	void LoadAssets();
	void LoadLevel();
	void Setup();
	void Run();
	void Update();
	void Render();
};
