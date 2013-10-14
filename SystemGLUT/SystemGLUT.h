#ifndef SYSTEMGLUT_H
#define SYSTEMGLUT_H

#include "../GLApp.h"

#define DEFAULT_WINDOW_WIDTH    1280
#define DEFAULT_WINDOW_HEIGHT    720

void InitGL();
void RunGL( GLApp& app );

int GetWindowWidth();
int GetWindowHeight();

#endif
