#include "IA.h"

double calcAngle(Vec2 u, Vec2 v)
{
	return acos((u.x * v.x + u.y * v.y) / (sqrt(u.x* u.x + u.y * u.y) * sqrt(v.x*v.x + v.y*v.y)));
}

unsigned char isNearWall(Node* node, unsigned int x, unsigned int y)
{
	return node->x < x || WORLD_X - node->x < x || node->y < y || WORLD_Y - node->y < y;
}

void InitMap(int x, int y)
{
	map = (unsigned char**)malloc(y / DIV_SCOUT * sizeof(unsigned char *));
	for(int i = 0; i < y / DIV_SCOUT; i++)
		*(map + i) = (unsigned char*)malloc(x / DIV_SCOUT * sizeof(unsigned char));

	for(int i = 0; i < y / DIV_SCOUT; i++)
		for(int j = 0; j < x / DIV_SCOUT; j++)
			map[i][j] = 0;

	initMap = 1;
	saved_brebie = NULL;
	saved_berger = NULL;

	WORLD_X = x;
	WORLD_Y = y;

	RDV.x = WORLD_X / 2;
	RDV.y = WORLD_Y / 2;

	printf("[BOT] Map set to (%d, %d)\n", x, y);
}

void InitIA()
{
	nodes = NULL;

	ticks = 0;

	iaStatus = EXPLORE;

	purple_status = GOTO;
	memset(&purple_communication_array, 0, sizeof(Vec2) * 20);
	purple_communication_target_id = 0;
}

Node* getHighestId(char* name)
{
	NodeStack* tmp = nodes;
	unsigned short maxID = 0;
	while(tmp != NULL)
	{
		if(tmp->node != NULL && strcmp(name, tmp->node->name) == 0)
			maxID = tmp->node->nodeID;
		tmp = tmp->next;
	}

	return NodeStack_get(nodes, maxID);
}

void UpdateNodes(unsigned char* data)
{
	//NodeStack_clear(nodes);
	unsigned int totalNameLength = 0;
	size_t NodeSize = 18; //sizeof(Node) - sizeof(char*)

	unsigned short deadSize;
	memcpy(&deadSize, data, sizeof(unsigned short));

	unsigned int startNodePos = 2 + 2 * deadSize * sizeof(int);
	unsigned int end;
	memcpy(&end, data + startNodePos, sizeof(unsigned int));

	int i = 0;
	while(end != 0)
	{
		unsigned char* pos = data + startNodePos + i * 18 + totalNameLength;
		Node* node = malloc(sizeof(Node));
		node->time = 0;

		memcpy(node, pos, 18);

		if(node->flags & 0x8)
		{
			node->type = PLAYER;
			size_t nameLength = strlen(pos + NodeSize); //taille du nom
			node->name = malloc(nameLength+1); //on aloue la memoire pour le nom
			strcpy(node->name, data + startNodePos + (i+1)*NodeSize + totalNameLength); //on copie le nom
			totalNameLength += nameLength+1;//on augment la taille total des noms
		}
		else if(node->flags&0x1)
			node->type = VIRUS;
		else
			node->type = FOOD;
		
		NodeStack_update(&nodes, node);

		//debugNode(node);

		memcpy(&end, data + startNodePos + (i+1)*(NodeSize) + totalNameLength, sizeof(unsigned int)); //la nouvelle fin (check si c'est 0)
		i++;
	}

	player = getHighestId(BotName);

	unsigned int new_pos = startNodePos + i*(NodeSize) + totalNameLength + sizeof(unsigned int); //nouvelle pos aprés avoir lu les cellules

	unsigned short nbDead; //nombre de cellule morte depuis la derniére fois
	memcpy(&nbDead, data + new_pos, sizeof(unsigned short)); //copie

	for(int j = 0; j < nbDead; j++) //pour chaque cellule morte
	{		
		unsigned int nodeID;
		memcpy(&nodeID, data + new_pos + sizeof(unsigned short) + j * sizeof(unsigned int), sizeof(unsigned int)); //on prend l'id
		if(player != NULL && nodeID == player->nodeID)
		{
			printf("delete player ???\n");
			player = NULL;
		}
		nodes = NodeStack_remove(nodes, nodeID); //on suprime de notre liste
	}

	//printNodeStack(nodes);
}

void Move(struct lws *wsi, Vec2 pos)
{
	//printf("Moving to %d, %d\n", pos.x, pos.y);
	unsigned char* packet = malloc(13);
	memset(packet, 0, 13);
	*packet = 16;

	memcpy(packet+1, &pos, sizeof(pos));

	sendCommand(wsi, packet, 13);
}

Vec2f unitarise(Vec2 vec)
{
	Vec2f ret;
	ret.x = vec.x / norme(vec);
	ret.y = vec.y / norme(vec);
	return ret;
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

Vec2 getRDVPointBlue()
{
	Vec2 rdv1; rdv1.x = RDV.x - 50; rdv1.y = RDV.y - 50;
	Vec2 rdv2; rdv2.x = RDV.x + 50; rdv2.y = RDV.y - 50;
	Vec2 rdv3; rdv3.x = RDV.x - 50; rdv3.y = RDV.y + 50;
	Vec2 rdv4; rdv4.x = RDV.x + 50; rdv4.y = RDV.y + 50;

	Vec2 rdvs[4] = {rdv1, rdv2, rdv3, rdv4};

	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node == player)
		{
			tmp = tmp->next;
			continue;
		}

		for(int i = 0; i < 4; i++)
		{
			if(tmp->node != NULL && equalsVec2(rdvs[i], GetNodePos(tmp->node)))
			{
				if(strcmp(tmp->node->name, "blue") == 0)
					continue;
				else
					return rdvs[i];
			}
		}		

		tmp = tmp->next;
	}

	return rdvs[0];
}

Vec2 getRDVPointPurple()
{
	Vec2 rdv1; rdv1.x = RDV.x - 50; rdv1.y = RDV.y - 50;
	Vec2 rdv2; rdv2.x = RDV.x + 50; rdv2.y = RDV.y - 50;
	Vec2 rdv3; rdv3.x = RDV.x - 50; rdv3.y = RDV.y + 50;
	Vec2 rdv4; rdv4.x = RDV.x + 50; rdv4.y = RDV.y + 50;

	Vec2 rdvs[4] = {rdv1, rdv2, rdv3, rdv4};

	unsigned char id = 0;

	NodeStack* tmp = nodes;
	while(tmp != NULL)
	{
		if(tmp->node == player)
		{
			tmp = tmp->next;
			continue;
		}

		for(int i = 0; i < 4; i++)
		{
			if(tmp->node != NULL && equalsVec2(rdvs[i], GetNodePos(tmp->node)))
			{
				if(strcmp(tmp->node->name, "blue") == 0)
					continue;
				else if(strcmp(tmp->node->name, "purple") == 0)
					continue;
			}
		}		

		tmp = tmp->next;
	}

	return rdvs[0];
}

Vec2 rotate(Vec2 vec, double angle)
{
	Vec2 ret;
	ret.x = vec.x * cos(angle) - vec.y * sin(angle);
	ret.y = (vec.y) * sin(angle) + vec.x * sin(angle);
	return ret;
}

unsigned char inRange(Vec2 target, Node* berger)
{
	return (berger->x - target.x < CARRE && berger->x - target.x > -CARRE && berger->y - target.y < CARRE && berger->y - target.y > -CARRE);
}

void show_debug_target(Vec2 target)
{
	Vec2 start; start.x = target.x - CARRE / 2; start.y = target.y - CARRE / 2;
	Vec2 end; end.x = target.x + CARRE / 2; end.y = target.y + CARRE / 2;

	drawDebugRect(World2Screen(start), World2Screen(end), 255, 0, 0);
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
	Vec2 base;
	base.x = BASE_X;
	base.y = BASE_Y;

	switch(purple_status)
	{
	case GOTO:
		direction.x = 0; direction.y = 0;

		if(distance(GetNodePos(player), RDV) < 150)
		{
			if(distance(getRDVPointBlue(), GetNodePos(player)) == 0)
				purple_status = LISTEN;
			else
			{
				Move(wsi, getRDVPointBlue());
				drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(getRDVPointBlue()), 255, 0, 0);
			}
		}
		else
		{
			Move(wsi, RDV);
			drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(RDV), 255, 0, 0);
		}

		if((brebie = brebie_in_fov()) != NULL && distance(GetNodePos(brebie), base)> 900){
			direction = GetNodePos(brebie);
			purple_status = BRING_BACK;
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
			//show_path();

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
				bring_back(wsi, brebie);
			}
			else
			{
				Vec2 new_pos;
				new_pos.x = player->x + direction.x;
				new_pos.y = player->y + direction.y;
				Move(wsi, new_pos);
				drawDebugLine(World2Screen(GetNodePos(player)), World2Screen(new_pos), 0, 0, 255);
				if(isNearWall(player, 250, 150))
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
		}
		break;
	}
}

 void IARecv(unsigned char* payload)
 {
    unsigned char opcode = payload[0];
	switch(opcode)
	{
	case 16:
		UpdateNodes(payload+1);
		//printf("Update node\n");
        break;

	case 17:
		//printf("View Update\n");
		break;

	case 18:
		//printf("Reset all Cells\n");
		//NodeStack_clear(nodes);
		break;

	case 20:
		//printf("Reset owned cells\n");
		//NodeStack_clear(playerNodes);
		break;

	case 21:
		//printf("Draw debug line\n");
		break;

	case 32:
		//printf("Owns blob\n");
		//AddNode(payload+1);
		break;

	case 49:
		//printf("FFA Leaderboard\n");
		break;

	case 50:
		//printf("Team Leaderboard\n");
		break;

	case 64:
		if(initMap == 0)
		{
			double a,b,c,d;
			memcpy(&a, payload+1, 8);
			memcpy(&b, payload+1+8, 8);
			memcpy(&c, payload+1+2*8, 8);
			memcpy(&d, payload+1+3*8, 8);
			InitMap((int)c, (int)d);
		}		
		break;

	case 72:
		//printf("HelloHelloHello\n");
		break;

	case 240:
		//printf("Message length\n");
		break;

	case 0:
		break;

	default:
		printf("Unknown opcode : %x\n", opcode);
		break;
	}
 }
