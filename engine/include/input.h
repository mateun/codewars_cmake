#pragma once

struct MouseInfo {
	unsigned int x;
	unsigned int y;
};

struct InputEvent {
	unsigned int type;
	MouseInfo mouseInfo;

};