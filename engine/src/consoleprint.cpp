#include "stdafx.h"
#include <consoleprint.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <Windows.h>

static FILE* hf_out = nullptr;

void initConsole() {
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 0);


	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 0);
	*stdin = *hf_in;
}

void cwprintf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(hf_out, format, args);
	va_end(args);
}