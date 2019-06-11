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

	pWindow = SDL_CreateWindow("AgoDog", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGTH, SDL_WINDOW_SHOWN);

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

Vec2 getZoom()
{
	Vec2 ret;
	memset(&ret, 0, sizeof(Vec2));

	if(isSpectator)
	{
		ret.x = WORLD_X;
		ret.y = WORLD_Y;
		return ret;
	}

	if(player == NULL)
		return ret;

	double factor = pow(min(64.0 / player->size, 1), 0.4);
  	ret.x = WINDOW_WIDTH / factor;
  	ret.y = WINDOW_HEIGTH / factor;

  	return ret;
}

Circle Node2Circle(Node* node)
{
	Vec2 zoom = getZoom();
	Circle ret;
	ret.radius = node->size * WINDOW_HEIGTH / zoom.y;
	ret.x = node->x;
	ret.y = node->y;
	ret.color.r = node->R;
	ret.color.g = node->G;
	ret.color.b = node->B;
	ret.color.a = 255;
	return ret;
}

Vec2 World2Screen(Vec2 pos)
{
	Vec2 ret;
	memset(&ret, 0, sizeof(Vec2));

	if(isSpectator == 1)
	{
		ret.x = pos.x * WINDOW_WIDTH / 9000;
		ret.y = pos.y * WINDOW_HEIGTH / 6000;
	}
	else
	{
		if(player == NULL)
			return ret;

		Vec2 zoom = getZoom();

		Vec2 playerPos = GetNodePos(player);

		ret.x = (pos.x - playerPos.x + zoom.x / 2) * WINDOW_WIDTH / zoom.x;
		ret.y = (pos.y - playerPos.y + zoom.y / 2) * WINDOW_HEIGTH / zoom.y;
	}

	return ret;
}

void DrawAllNodes()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL)
		{
			Circle nodeCircle = Node2Circle(tmp->node);

			Vec2 nodePos = GetNodePos(tmp->node);
			nodePos = World2Screen(nodePos);

			nodeCircle.x = nodePos.x;
			nodeCircle.y = nodePos.y;

			DrawCircle(&nodeCircle);

			if(tmp->node->type == PLAYER)
			{
				char* toWrite = malloc(strlen(tmp->node->name) + 1 + 6);
				sprintf(toWrite, "%s [%d]", tmp->node->name, tmp->node->size);
				SDL_Color color;
				color.r = 255 - tmp->node->R;
				color.g = 255 - tmp->node->G;
				color.b = 255 - tmp->node->B;
				color.a = 255;
				SDL_Surface* textSurface = TTF_RenderUTF8_Blended(pFont, toWrite, color);
				SDL_Texture* texture = SDL_CreateTextureFromSurface(pRenderer, textSurface);

				Vec2 zoom = getZoom();
				unsigned short nodeSize = tmp->node->size;
				SDL_Rect rekt;
				rekt.x = nodePos.x - nodeSize / 2;
				rekt.y = nodePos.y;
				rekt.w = nodeSize * WINDOW_WIDTH / zoom.x;
				rekt.h = nodeSize * WINDOW_HEIGTH / zoom.y;

				SDL_RenderCopy(pRenderer, texture, NULL, &rekt);

				SDL_FreeSurface(textSurface);
				SDL_DestroyTexture(texture);
			}
		}
		tmp = tmp->next;
	}
}

void Draw()
{
	//drawWalls();

	DrawAllNodes();
}

void Render()
{
	SDL_SetRenderDrawColor(pRenderer, 125, 125, 125, 255);

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
