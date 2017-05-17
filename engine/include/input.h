#pragma once

struct MouseInfo {
	unsigned int x;
	unsigned int y;
};

struct FrameInput {
	bool* keyState;
	bool mouse1Down = false;
	bool mouse1Up = false;
	bool mouse2Down = false;
	bool mouse2Up = false;
	int relMouseMovX = false;
	int relMouseMovY = false;
};