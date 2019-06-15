#include "Scout.h"

Node* brebie_in_fov()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strncmp(tmp->node->name, "bot", 3) == 0 && !NodeStack_find(saved_brebie, tmp->node->nodeID)) //&& notInBuff(saw_id, 10, tmp->node->nodeID))
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
		if(tmp->node != NULL && strcmp(tmp->node->name, "purple") == 0)// && NodeNotInBuff(berger_id, 3, tmp->node))
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

void updateLifeTime()
{
    for(unsigned char i = 0; i < 3; i++)
    {
        berger_id[i].time--;
        if(berger_id[i].time == 0)
        {
            berger_i--;
            memset(berger_id+i*sizeof(Timed_Node), 0, sizeof(Timed_Node));
        }
    }
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

void Scout(struct lws* wsi)
{
	if(player == NULL || initMap == 0)
		return;

	Node* brebie = NULL;
	Node* berger = NULL;

	updateBrebieStack();

    updateLifeTime();

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
		if((berger = berger_in_fov()) != NULL)
		{
			Move(wsi, GetNodePos(berger));
			if(equalsVec2(GetNodePos(berger), GetNodePos(player)))
			{
				iaStatus = COMMUNICATING;
                blue_ticks_start = ticks;
				printf("Same pos, communication...\n");
			}
		}
		else
			Move(wsi, RDV);
		break;
	case COMMUNICATING:
		//printf("[BOT-Blue] Communicating state, deltatime=%d\n", ticks - blue_ticks_start);
		printNodeStack(saved_brebie);
		brebie = NodeStack_getNearest(saved_brebie, player);
		debugNode(brebie);
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