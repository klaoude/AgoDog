#include "Scout.h"

Node* brebie_in_fov()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strncmp(tmp->node->name, "bot", 3) == 0) //&& notInBuff(saw_id, 10, tmp->node->nodeID))
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

void Scout(struct lws* wsi)
{
	if(player == NULL || initMap == 0)
		return;

	Node* brebie = NULL;
	Node* berger = NULL;

    updateLifeTime();

	switch(iaStatus)
	{
	case EXPLORE:
		if((brebie = brebie_in_fov()) != NULL)
		{
			Node* brebie_copie = malloc(sizeof(Node));
			memcpy(brebie_copie, brebie, sizeof(Node));
			NodeStack_update(&saved_brebie, brebie_copie);
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
			//printf("[Bot-Blue] Found berger !\n");
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

		if(ticks - blue_ticks_start >= 2  && saved_brebie != NULL)
		{
			if(ticks - blue_ticks_start >= 15)
			{
                printf("[Bot-Blue] Sended direction (%d, %d)\n", saved_brebie->node->x, saved_brebie->node->y);
				saw_id[saw_i++] = saved_brebie->node->nodeID;
				saved_brebie = NodeStack_remove(saved_brebie, saved_brebie->node->nodeID);
                berger_id[berger_i].node = berger;
                berger_id[berger_i++].time = 200;
                if(ticks - blue_ticks_start >= 20) ;
                //else iaStatus = EXPLORE;
			}
			else
			{
				Move(wsi, GetNodePos(saved_brebie->node));
				//debugNode(saved_brebie->node);
			}
		}
		break;
	}	
}