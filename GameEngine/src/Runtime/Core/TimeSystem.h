#pragma once

const int FPS = 60;
const int FRAMETIME_MS = 1000 / FPS;

class TimeSystem
{
private:
	unsigned int msLastFrame;
	float dt;
	bool capFrameRate;
	float time;

public:
	TimeSystem();
	void Update();
	float GetTime();
	float GetdeltaTime();
};