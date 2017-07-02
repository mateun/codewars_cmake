#include "stdafx.h"
#include <logging.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

static FILE* logfile = nullptr;

void flog(const char* format, ...) {
	if (logfile == nullptr) {
		logfile = fopen("game.log", "w");
	}
	va_list args;
	va_start(args, format);
	vfprintf(logfile, format, args);
	va_end(args);
}