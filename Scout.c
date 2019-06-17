#include "Scout.h"

Node* brebie_in_fov()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strncmp(tmp->node->name, "bot", 3) == 0 && !NodeStack_find(saved_brebie, tmp->node->nodeID))
			return tmp->node;
		tmp = tmp->next;
	}
	return NULL;
}

Node* berger_in_fov()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strcmp(tmp->node->name, "purple") == 0)
			return tmp->node;
		tmp = tmp->next;
	}
	return NULL;
}

Vec2 GetNextUnseenRegion()
{
	Vec2 ret;
	for(int y = WORLD_Y / 100 - 1; y > 0; y--)
	{
		for(int x = WORLD_X / 100 - 1; x > 0; x--)
		{
			if(map[y][x] == 0)
			{
				ret.x = x * 100;
				ret.y = y * 100;
				return ret;	
			}
		}
	}
}

Vec2 WorldtoMap(Vec2 pos)
{
	Vec2 ret;
	ret.x = pos.x / 100;
	ret.y = pos.y / 100;
	return ret;
}

void updateBrebieStack()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strncmp(tmp->node->name, "bot", 3) == 0)
		{
			Node* cpy = malloc(sizeof(Node));
			memcpy(cpy, tmp->node, sizeof(Node));
			NodeStack_update(&saved_brebie, cpy);	
		}

		tmp = tmp->next;
	}
}

void updateBergerStack()
{
	NodeStack* tmp = saved_berger;
	while(tmp != NULL)
	{
		if(tmp->node != NULL)
			tmp->node->time--;
		tmp = tmp->next;
	}
}

Node* getNearestBerger(Node* node)
{
	NodeStack* tmpp = NULL;
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		debugNode(tmp->node);
		if(tmp->node != NULL && strcmp(tmp->node->name, "purple") == 0)
		{
			Node* cpy = malloc(sizeof(Node));
			memcpy(cpy, tmp->node, sizeof(Node));
			NodeStack_update(&tmpp, cpy);
			printf("Saving %p\n", cpy);
		}
		tmp = tmp->next;
	}
	printNodeStack(nodes);
	printNodeStack(tmpp);
	Node near = *NodeStack_getNearest(tmpp, node);
	debugNode(&near);
	NodeStack_clear(tmpp);
	return &near;
}

void Scout(struct lws* wsi)
{
	if(player == NULL || initMap == 0)
		return;

	Node* brebie = NULL;
	Node* berger = NULL;

	updateBrebieStack();
	updateBergerStack();

	switch(iaStatus)
	{
	case EXPLORE:
		if(saved_brebie != NULL)
		{
			iaStatus = GOTORDV;
			printf("[BOT-Blue] Brebie found !!\n");
		}
		else
		{
			Vec2 pos = GetNodePos(player);
			Vec2 coord = WorldtoMap(pos);

			if(map[coord.y][coord.x] == 0)
			{
				map[coord.y][coord.x] = 1;
			}

			Vec2 next = GetNextUnseenRegion();
			Move(wsi, next);
		}		
		break;
	case GOTORDV:
		printf("gotordv\n");
		if(distance(RDV, GetNodePos(player)) < 450)
		{
			Vec2 rdv = getRDVPoint();
			printf("[Bot-Blue] Near rdv point, goto (%d, %d)\n", rdv.x, rdv.y);
			if(equalsVec2(rdv, GetNodePos(player)))
			{
				printf("[Bot-Blue] Arrived at RDV. Waiting for berger...\n");
				if((berger = berger_in_fov()) != NULL && equalsVec2(GetNodePos(berger), GetNodePos(player)))
				{
					iaStatus = COMMUNICATING;
					blue_ticks_start = ticks;
					berger->time = 1000;
					NodeStack_update(&saved_berger, berger);
					printf("[Bot-Blue] Same pos, communication...\n");
				}
			}
			else
				Move(wsi, rdv);
		}
		/*if((berger = berger_in_fov()) != NULL)
		{
			if(!NodeStack_find(saved_berger, berger->nodeID) && equalsVec2(GetNodePos(berger), GetNodePos(player)))
			{
				iaStatus = COMMUNICATING;
                blue_ticks_start = ticks;
				berger->time = 1000;
				NodeStack_update(&saved_berger, berger);
				printf("Same pos, communication...\n");
			}
		}*/
		else
			Move(wsi, RDV);
		break;
	case COMMUNICATING:
		//printf("[BOT-Blue] Communicating state, deltatime=%d\n", ticks - blue_ticks_start);
		//printNodeStack(saved_brebie);
		brebie = NodeStack_getNearest(saved_brebie, player);
		//debugNode(brebie);
		if(ticks - blue_ticks_start >= 2 && brebie != NULL)
		{
			if(ticks - blue_ticks_start >= 15)
			{
                printf("[Bot-Blue] Sended direction (%d, %d)\n", brebie->x, brebie->y);
				saved_brebie = NodeStack_remove(saved_brebie, brebie->nodeID);
                iaStatus = EXPLORE;
			}
			else
			{
				Move(wsi, GetNodePos(brebie));
				//debugNode(saved_brebie->node);
			}
		}
		break;
	}	
}