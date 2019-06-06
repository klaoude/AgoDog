#include "UI.h"
//#include "IA.h"

void DrawCircle(Circle* circle)
{
	filledCircleRGBA(pRenderer, circle->x, circle->y, circle->radius, circle->color.r, circle->color.g, circle->color.b, circle->color.a);
}

void drawDebugCircle(int x, int y, short radius, char r, char g, char b)
{
	aacircleRGBA(pRenderer, x, y, radius, r, g, b, 255);
}

int InitUI()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return 0;

	pWindow = SDL_CreateWindow("AgoBot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGTH, SDL_WINDOW_SHOWN);

	if(pWindow == NULL)
		return 0;

	pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);
	if(pRenderer == NULL)
		return 0;

	if(TTF_Init() == -1)
		return 0;

	pFont = TTF_OpenFont("Roboto-Regular.ttf", 60);

	return 1;
}

void Clear()
{
	SDL_RenderClear(pRenderer);
}

void Draw()
{
	//drawWalls();

	//DrawAllNodes();
}

void Render()
{
	SDL_SetRenderDrawColor(pRenderer, 225, 225, 225, 255);

	SDL_RenderPresent(pRenderer);
}

void Loop(int* exit)
{
	SDL_Event evnt;

	while(SDL_PollEvent(&evnt))
	{
		if(evnt.type == SDL_QUIT)
			*exit = 1;
	}

	Render();
}