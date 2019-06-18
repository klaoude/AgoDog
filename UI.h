#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "Utils.h"
#include "IA.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGTH 450

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

void drawDebugLine(Vec2 start, Vec2 end, char r, char g, char b);
void drawDebugCircle(int x, int y, short radius, char r, char g, char b);
void drawDebugRect(Vec2 start, Vec2 end, char r, char g, char b);

Vec2 World2Screen(Vec2 pos);

int InitUI();
void Loop(int* exit);
void Clear();
void Draw();

void minimap_clear();
void minimap_draw();

#endif