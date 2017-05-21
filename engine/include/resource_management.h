#pragma once
#include <d3d11.h>

template<typename T>
void safeRelease(T** o) {
	if (*o) {
		(*o)->Release();
		*o = nullptr;
	}

}