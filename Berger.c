#include "Berger.h"
#include "IA.h"

void InitBerger()
{
	berger_status = GOTO;
	memset(&berger_communication_array, 0, sizeof(Vec2) * 20);
	berger_communication_target_id = 0;

	berger_name = "yellow";
}

Node* scout_in_fov()
{
	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strcmp(tmp->node->name, "blue") == 0)
			return tmp->node;
		tmp = tmp->next;
	}
	return NULL;
}
 
Vec2f GetTarget(Node* brebie)
{
	Vec2f ret; memset(&ret, 0, sizeof(Vec2f));
	if(brebie == NULL)
		return ret;

	Vec2 brebie_pos = GetNodePos(brebie);
	Vec2 base; 
	base.x = BASE_X;
	base.y = BASE_Y;

	Vec2 vect; 
	vect.x = brebie_pos.x - base.x;
	vect.y = brebie_pos.y - base.y;

	return unitarise(vect);
}

void show_path()
{
	printf("[Bot-Purple] Trajet viewer !\n");
	for(int i = 0; i < TICKS_LISTEN; i++)
		printf("(%d, %d) -> ", berger_communication_array[i].x, berger_communication_array[i].y);
	printf("\n-------------------------------------\n");
}

Vec2 process_path()
{
	Vec2 sorted_path[TICKS_LISTEN] = {0};
	Vec2 direction = RDV;

	unsigned int sorted_counter = 0;

	for(int i = 0; i < TICKS_LISTEN; i++)
		if(!Vec2_isZero(berger_communication_array[i]) && !equalsVec2(berger_communication_array[i-1], berger_communication_array[i]))
			sorted_path[sorted_counter++] = berger_communication_array[i];

	//sorted_counter--;

	if(sorted_counter == 0)
		return direction;

	
	direction.x = sorted_path[sorted_counter-1].x - sorted_path[0].x;
	direction.y = sorted_path[sorted_counter-1].y - sorted_path[0].y;

	printf("return of process : (%d, %d)\n", direction.x, direction.y);

	return direction;
}

Vec2 getRDVPointPurple()
{
	Vec2 rdv1; rdv1.x = RDV.x - 100; rdv1.y = RDV.y - 100;
	Vec2 rdv2; rdv2.x = RDV.x + 100; rdv2.y = RDV.y - 100;
	Vec2 rdv3; rdv3.x = RDV.x - 100; rdv3.y = RDV.y + 100;
	Vec2 rdv4; rdv4.x = RDV.x + 100; rdv4.y = RDV.y + 100;

	Vec2 rdvs[4] = {rdv1, rdv2, rdv3, rdv4};

	unsigned char id = 0;

	for(int i = 0; i < 4; i++)
	{
		Node* node = isNodeHere(rdvs[i]);

		if(node == player)
			return rdvs[i];

		if(node != NULL && strcmp(node->name, "blue") == 0)
			return rdvs[i];
	}

	for(int i = 0; i < 4; i++)
	{
		Node* node = isNodeHere(rdvs[i]);

		if(node == NULL)
			return rdvs[i];
	}		

	return RDV;
}

Vec2 fixTarget(Vec2* target)
{
	if(target->x < 25)
		target->x = 25;
	else if(target->x > WORLD_X - 25)
		target->x = WORLD_X - 25;
	if(target->y < 25)
		target->y = 25;
	else if(target->y > WORLD_Y - 25)
		target->y = WORLD_Y - 25;
}

void bring_back(struct lws* wsi, Node* brebie)
{
	berger_follow_id = brebie->nodeID;
	Vec2 U,V;

	Vec2f unit = GetTarget(brebie);
	unit.x = brebie->x + unit.x * (RAYON_BERGER + OFFSET);
	unit.y = brebie->y + unit.y * (RAYON_BERGER + OFFSET);
	Vec2 target = Vec2ftoVec2(unit);
	fixTarget(&target);
	Vec2 coord = World2Screen(target);
	drawDebugCircle(coord.x, coord.y, 10, 255, 255, 0);

	Vec2 brebie_screen = World2Screen(GetNodePos(brebie));
	drawDebugCircle(brebie_screen.x, brebie_screen.y, RAYON_BERGER + OFFSET, 0, 0, 0);

	Vec2 base; base.x = BASE_X; base.y = BASE_Y;
	if(distance(GetNodePos(brebie), base) < 900)
	{
		berger_status = GOTO;
		return;	
	}

	if(!equalsVec2(target, GetNodePos(player)))
	{
		if(distance(GetNodePos(brebie), GetNodePos(player)) < RAYON_BERGER)
		{
			Vec2 vec;
			vec.x = brebie->x - player->x;
			vec.y = brebie->y - player->y;

			drawDebugLine(World2Screen(GetNodePos(player)), brebie_screen, 255, 255, 0);

			Vec2 V; 
			V.x = target.x - player->x;
			V.y = target.y - player->y;

			vec = calcAngle(vec, V) > 0 ? rotate(vec, M_PI / 2) : rotate(vec, -M_PI / 2);

			Vec2 direction;
			direction.x = vec.x + player->x;
			direction.y = vec.y + player->y;

			drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(direction), 255, 0, 255);
			Move(wsi, direction);
		}
		else
			Move(wsi, target);
	}
	else
	{
		berger_status = BRING_BACK; 
	}	
}

unsigned char isBrebieFree(Node* brebie)
{
	Vec2 base; base.x = BASE_X; base.y = BASE_Y;
	Node* berger = berger_in_fov();
	if(berger != NULL)
		return (player->nodeID < berger->nodeID && distance(GetNodePos(brebie), base) < 900);
	else
		return 1;
}

void Berger(struct lws* wsi)
{
	if(player == NULL)
		return;

	Node* berger = NULL;
	Node* brebie = NULL;
	Node* scout = NULL;

	static Vec2 direction;

	Vec2 base; base.x = BASE_X; base.y = BASE_Y;

	switch(berger_status)
	{
	case GOTO:
		direction.x = direction.y = 0;
		if((brebie = brebie_in_fov()) != NULL)
		{
			if(isBrebieFree(brebie))
			{
				direction = GetNodePos(brebie);
				berger_status = LOOKING;
			}
		}

		if(distance(GetNodePos(player), RDV) < 150)
		{
			Vec2 rdv = getRDVPointPurple();

			if((scout = scout_in_fov()) != NULL && equalsVec2(GetNodePos(player), GetNodePos(scout)))
			{
				berger_status = LISTEN;
				berger_communication_target_id = scout->nodeID;
				berger_ticks = ticks;
				return;
			}

			Move(wsi, rdv);
			drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(rdv), 255, 0, 255);
			return;
		}

		Move(wsi, RDV);
		drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(RDV), 255, 0, 0);
		break;
	
	case LISTEN:
		{
			Node* target = NodeStack_get(nodes, berger_communication_target_id);
			if(ticks - berger_ticks > TICKS_LISTEN)
			{
				show_path();

				direction = process_path();

				berger_status = LOOKING;
			}
			else if(target != NULL)
			{
				berger_communication_array[ticks - berger_ticks] = GetNodePos(target);
				drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(GetNodePos(target)), 0, 255, 0);
			}
		}
		break;
	
	case LOOKING:
		if(!Vec2_isZero(direction))
		{
			if((brebie = brebie_in_fov()) != NULL && isBrebieFree(brebie))
			{
				bring_back(wsi, brebie);
				return;
			}
			else if(isNearWall(player, 50, 50))
				berger_status = GOTO;
			else
			{
				Vec2 new_pos;
				new_pos.x = player->x + direction.x;
				new_pos.y = player->y + direction.y;
				Move(wsi, new_pos);
				drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(new_pos), 0, 0, 255);
			}
		}
		else
			printf("[Bot-Berger] Erreur Looking without direction !\n");
		break;

	case BRING_BACK:
		if((brebie = brebie_in_fov()) != NULL)
		{
			Vec2f unit = GetTarget(brebie);
			unit.x = brebie->x + unit.x * (10);
			unit.y = brebie->y + unit.y * (10);
			Vec2 target = Vec2ftoVec2(unit);
			fixTarget(&target);
			Vec2 coord = World2Screen(target);
			drawDebugCircle(coord.x, coord.y, 10, 255, 255, 0);
			Move(wsi, target);
		}

		if(distance(GetNodePos(player), base) < 900)
		{
			berger_status = GOTO;
			berger_follow_id = 0;
		}
		break;
	}
}

/*
void Berger(struct lws* wsi)
{
	if(player == NULL)
		return;

	Node* scout = NULL;
	Node* target = NULL;
	static Vec2 direction;
	Node* brebie = NULL;
	Node* berger = NULL;
	Vec2 base;
	Vec2f point_targetf;
	Vec2 point_target;
	base.x = BASE_X;
	base.y = BASE_Y;

	static unsigned char toto = 0;

	switch(berger_status)
	{
	case GOTO:
		direction.x = 0; direction.y = 0;

		if((brebie = brebie_in_fov()) != NULL && distance(GetNodePos(brebie), base)> 900)
		{
			if((berger = berger_in_fov()) != NULL && distance(GetNodePos(brebie), GetNodePos(player)) < distance(GetNodePos(berger), GetNodePos(brebie)))
			{
				direction = GetNodePos(brebie);
				berger_status = BRING_BACK;
			}
			else if(berger == NULL)
			{
				direction = GetNodePos(brebie);
				berger_status = BRING_BACK;
			}
		}

		if(distance(GetNodePos(player), RDV) < 150)
		{
			Vec2 rdv = getRDVPointPurple();
			if(distance(rdv, GetNodePos(player)) == 0 && !equalsVec2(RDV, rdv))
				berger_status = LISTEN;
			else
			{
				Move(wsi, rdv);
				drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(rdv), 255, 0, 255);
			}
		}
		else
		{
			Move(wsi, RDV);
			drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(RDV), 255, 0, 0);
		}
		break;

	case LISTEN:
		if((scout = scout_in_fov()) != NULL && equalsVec2(GetNodePos(player), GetNodePos(scout)))
		{
			berger_status = GETTING_INFO;
			printf("[Bot-berger] Samepos as a scout, Start listening !\n");
			berger_ticks = ticks;
			berger_communication_target_id = scout->nodeID;
		}
		break;

	case GETTING_INFO:
		target = NodeStack_get(nodes, berger_communication_target_id);
		if(ticks - berger_ticks >= 20)
		{
			show_path();

			direction = process_path();

			berger_status = BRING_BACK;
		}
		else if(target != NULL)
		{
			berger_communication_array[ticks - berger_ticks] = GetNodePos(target);
			drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(GetNodePos(target)), 0, 255, 0);
		}
		break;

	case BRING_BACK:
		if(!Vec2_isZero(direction))
		{
			if((brebie = brebie_in_fov()) != NULL)
			{
				if((berger = berger_in_fov()) != NULL)
				{
					if(distance(GetNodePos(brebie), GetNodePos(player)) < distance(GetNodePos(berger), GetNodePos(brebie)))
						bring_back(wsi, brebie);
					else
						berger_status = GOTO;
				}
				else
					bring_back(wsi, brebie);
			}
			else
			{
				Vec2 new_pos;
				new_pos.x = player->x + direction.x;
				new_pos.y = player->y + direction.y;
				Move(wsi, new_pos);
				drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(new_pos), 0, 0, 255);
				if(isNearWall(player, 50, 50))
				{
					printf("[Bot-Purple] Too close to the wall !\n");
					berger_status = GOTO;
				}
			}
		}	
		break;
	
	case RAMENEZ:		
		if((brebie = brebie_in_fov()) != NULL)
		{
			Vec2f unit = GetTarget(brebie);
			unit.x = brebie->x + unit.x * (RAYON_BERGER - 10);
			unit.y = brebie->y + unit.y * (RAYON_BERGER - 10);
			Vec2 target = Vec2ftoVec2(unit);
			fixTarget(&target);
			Vec2 coord = World2Screen(target);
			drawDebugCircle(coord.x, coord.y, 10, 255, 255, 0);

			if(toto)
			{
				Move(wsi, target);
				if(equalsVec2(target, GetNodePos(player)))
					toto = 0;
				return;
			}		

			if(distance(GetNodePos(player), target) > 50)
			{
				toto = 1;
			}
			else
				Move(wsi, base);
		}
		if(distance(GetNodePos(player), base) < 900){
			berger_status = GOTO;
			berger_follow_id = 0;
		}
		break;
	}
}
*/