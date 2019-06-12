#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "Utils.h"
#include "IA.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGTH 600

typedef struct Color
{
	char r,g,b,a;
} Color;

typedef struct Circle
{
	unsigned int radius;
	int x, y;
	Color color;
} Circle;

unsigned char isUI;

SDL_Window* pWindow;
SDL_Renderer* pRenderer;

TTF_Font* pFont;

int InitUI();
void Loop(int* exit);
void Clear();
void Draw();

void minimap_clear();
void minimap_draw();

void drawDebugCircle(int x, int y, short radius, char r, char g, char b);

#endif