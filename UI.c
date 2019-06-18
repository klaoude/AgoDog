#include "UI.h"
//#include "IA.h"

void DrawCircle(Circle* circle)
{
	filledCircleRGBA(pRenderer, circle->x, circle->y, circle->radius, circle->color.r, circle->color.g, circle->color.b, circle->color.a);
}

void drawDebugLine(Vec2 start, Vec2 end, char r, char g, char b)
{
	aalineRGBA(pRenderer, start.x, start.y, end.x, end.y, r, g, b, 255);
}

void drawDebugCircle(int x, int y, short radius, char r, char g, char b)
{
	aacircleRGBA(pRenderer, x, y, radius, r, g, b, 255);
}

void drawDebugRect(Vec2 start, Vec2 end, char r, char g, char b)
{
	Vec2 pos1; pos1.x = start.x + end.x; pos1.y = start.x;
	drawDebugLine(start, pos1, r, g, b);

	drawDebugLine(pos1, end, r, g, b);

	pos1.x = start.x; pos1.y = start.y + end.y;
	drawDebugLine(end, pos1, r, g, b);

	drawDebugLine(pos1, start, r, g, b);
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
	ret.x = 1; ret.y = 1;

	if(!initMap)
	{
		ret.x = 1;
		ret.y = 1;
		return ret;
	}

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

void drawWalls()
{
	if(player == NULL)
		return;

	Vec2 topLeft, topRight, bottomLeft, bottomRight;
	topLeft.x = 0; topLeft.y = 0;
	topRight.x = WORLD_X; topRight.y = 0;
	bottomLeft.x = 0; bottomLeft.y = WORLD_Y;
	bottomRight.x = WORLD_X; bottomRight.y = WORLD_Y;

	Vec2 topLeftWall, topRightWall, bottomLeftWall, bottomRightWall;

	topLeftWall.x = topLeft.x;
	topLeftWall.y = topLeft.y;
	topRightWall.x = topRight.x;
	topRightWall.y = topRight.y;
	bottomLeftWall.x = bottomLeft.x;
	bottomLeftWall.y = bottomLeft.y;
	bottomRightWall.x = bottomRight.x;
	bottomRightWall.y = bottomRight.y;

	Vec2 playerPos = GetNodePos(player);

	topLeft = World2Screen(topLeft);
	topRight = World2Screen(topRight);
	bottomLeft = World2Screen(bottomLeft);
	bottomRight = World2Screen(bottomRight);

	topLeftWall = World2Screen(topLeftWall);
	topRightWall = World2Screen(topRightWall);
	bottomLeftWall = World2Screen(bottomLeftWall);
	bottomRightWall = World2Screen(bottomRightWall);

	drawDebugLine(topLeft, topRight, 0, 0, 0);
	drawDebugLine(topRight, bottomRight, 0, 0, 0);
	drawDebugLine(bottomRight, bottomLeft, 0, 0, 0);
	drawDebugLine(bottomLeft, topLeft, 0, 0, 0);

	drawDebugLine(topLeftWall, topRightWall, 0, 255, 255);
	drawDebugLine(topRightWall, bottomRightWall, 0, 255, 255);
	drawDebugLine(bottomRightWall, bottomLeftWall, 0, 255, 255);
	drawDebugLine(bottomLeftWall, topLeftWall, 0, 255, 255);
}

void DrawNode(Node* node)
{
	Circle nodeCircle = Node2Circle(node);

	Vec2 nodePos = GetNodePos(node);
	nodePos = World2Screen(nodePos);

	nodeCircle.x = nodePos.x;
	nodeCircle.y = nodePos.y;

	DrawCircle(&nodeCircle);
}

void DrawAllNodes()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL)
			DrawNode(tmp->node);

		tmp = tmp->next;
	}
}

void Draw()
{
	drawWalls();

	Node enclos;
	enclos.nodeID = 0;
	enclos.x = 0;
	enclos.y = WORLD_Y / 2;
	enclos.size = 900;
	enclos.R = 50;
	enclos.G = 50;
	enclos.B = 50;
	enclos.flags = 0x2;
	
	DrawNode(&enclos);

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
