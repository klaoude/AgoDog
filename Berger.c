#include "Berger.h"
#include "IA.h"

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
	for(int i = 0; i < 20; i++)
		printf("(%d, %d) -> ", purple_communication_array[i].x, purple_communication_array[i].y);
	printf("\n-------------------------------------\n");
}

Vec2 process_path()
{
	Vec2 sorted_path[20] = {0};
	Vec2 direction = RDV;

	unsigned int sorted_counter = 0;

	for(int i = 0; i < 20; i++)
	{
		if(!Vec2_isZero(purple_communication_array[i]) && !equalsVec2(purple_communication_array[i], RDV)
			&& !equalsVec2(purple_communication_array[i-1], purple_communication_array[i]))
		{
			sorted_path[sorted_counter++] = purple_communication_array[i];
		}
	}

	//sorted_counter--;

	
	direction.x =sorted_path[sorted_counter-1].x - sorted_path[0].x;
	direction.y = sorted_path[sorted_counter-1].y - sorted_path[0].y;

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
	if(target->x < 10)
		target->x = 10;
	else if(target->x > WORLD_X - 10)
		target->x = WORLD_X - 10;
	if(target->y < 10)
		target->y = 10;
	else if(target->y > WORLD_Y - 10)
		target->y = WORLD_Y - 10;
}

void bring_back(struct lws* wsi, Node* brebie)
{
	purple_follow_id = brebie->nodeID;
	Vec2 U,V;

	Vec2f unit = GetTarget(brebie);
	unit.x = brebie->x + unit.x * (RAYON_BERGER + OFFSET + 40);
	unit.y = brebie->y + unit.y * (RAYON_BERGER + OFFSET + 40);
	Vec2 target = Vec2ftoVec2(unit);
	fixTarget(&target);
	Vec2 coord = World2Screen(target);
	drawDebugCircle(coord.x, coord.y, 10, 255, 255, 0);

	Vec2 brebie_screen = World2Screen(GetNodePos(brebie));
	drawDebugCircle(brebie_screen.x, brebie_screen.y, RAYON_BERGER + OFFSET, 0, 0, 0);

	if(isNearWall(brebie, 10, 10))
	{
		Move(wsi, GetNodePos(brebie));
		return;
	}

	if(!equalsVec2(target, GetNodePos(player)))
	{
		if(distance(GetNodePos(brebie), GetNodePos(player)) < RAYON_BERGER + OFFSET)
		{
			Vec2 vec;
			vec.x = brebie->x - player->x;
			vec.y = brebie->y - player->y;

			drawDebugLine(World2Screen(GetNodePos(player)), brebie_screen, 255, 255, 0);

			Vec2 V; 
			V.x = target.x - player->x;
			V.y = target.y - player->y;

			vec = calcAngle(vec, V) > 0 ? rotate(vec, M_PI / 2 + 0.09) : rotate(vec, -M_PI / 2 - 0.09);

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
		purple_status = RAMENEZ;
	}	
}

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

	switch(purple_status)
	{
	case GOTO:
		direction.x = 0; direction.y = 0;

		if(distance(GetNodePos(player), RDV) < 150)
		{
			Vec2 rdv = getRDVPointPurple();
			if(distance(rdv, GetNodePos(player)) == 0 && !equalsVec2(RDV, rdv))
				purple_status = LISTEN;
			else
			{
				Move(wsi, rdv);
				drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(rdv), 255, 0, 255);
                if(equalsVec2(GetNodeplayer))
			}
		}
		else
		{
			Move(wsi, RDV);
			drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(RDV), 255, 0, 0);
		}

		if((brebie = brebie_in_fov()) != NULL && distance(GetNodePos(brebie), base)> 900)
		{
			//berger = berger_in_fov();
			/*if(berger == NULL)
			{
				direction = GetNodePos(brebie);
				purple_status = BRING_BACK;
			}
			if(berger != NULL && distance(GetNodePos(berger), GetNodePos(brebie)) > distance(GetNodePos(player), GetNodePos(brebie)))
			{*/
				direction = GetNodePos(brebie);
				purple_status = BRING_BACK;
			//}


		}
		break;

	case LISTEN:
		if((scout = scout_in_fov()) != NULL && equalsVec2(GetNodePos(player), GetNodePos(scout)))
		{
			purple_status = GETTING_INFO;
			printf("[Bot-Purple] Samepos as a scout, Start listening !\n");
			purple_ticks = ticks;
			purple_communication_target_id = scout->nodeID;
		}
		break;

	case GETTING_INFO:
		target = NodeStack_get(nodes, purple_communication_target_id);
		if(ticks - purple_ticks >= 20)
		{
			show_path();

			direction = process_path();

			purple_status = BRING_BACK;
		}
		else if(target != NULL)
		{
			purple_communication_array[ticks - purple_ticks] = GetNodePos(target);
			drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(GetNodePos(target)), 0, 255, 0);
		}
		break;

	case BRING_BACK:
		if(!Vec2_isZero(direction))
		{
			if((brebie = brebie_in_fov()) != NULL)
			{
				point_targetf = GetTarget(brebie);
				point_targetf.x = brebie->x + point_targetf.x * (RAYON_BERGER + OFFSET + 40);
				point_targetf.y = brebie->y + point_targetf.y * (RAYON_BERGER + OFFSET + 40);
				point_target = Vec2ftoVec2(point_targetf);
				berger = berger_in_fov();					
				if(berger == NULL)
				{
					bring_back(wsi, brebie); 
				}
				else if((berger != NULL) && distance(GetNodePos(berger), point_target) > distance(GetNodePos(player), point_target))
					{
						bring_back(wsi, brebie); 
					}
			}
			else
			{
				Vec2 new_pos;
				new_pos.x = player->x + direction.x;
				new_pos.y = player->y + direction.y;
				Move(wsi, new_pos);
				drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(new_pos), 0, 0, 255);
				if(isNearWall(player, 20, 20))
					purple_status = GOTO;
			}
		}	
		break;
	
	case RAMENEZ:		
		if((brebie = brebie_in_fov()) != NULL)
		{
			Vec2f unit = GetTarget(brebie);
			unit.x = brebie->x + unit.x * (RAYON_BERGER - 21);
			unit.y = brebie->y + unit.y * (RAYON_BERGER - 21);
			Vec2 target = Vec2ftoVec2(unit);
			fixTarget(&target);
			Vec2 coord = World2Screen(target);
			drawDebugCircle(coord.x, coord.y, 10, 255, 255, 0);
			if(distance(GetNodePos(player), target) > 50){
				Move(wsi, target);
			}
			else{
				Move(wsi, base);
			}
		}
		if(distance(GetNodePos(player), base) < 900){
			purple_status = GOTO;
			purple_follow_id = 0;
		}
		break;
	}
}