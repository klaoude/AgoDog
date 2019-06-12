#include "IA.h"

void InitMap(int x, int y)
{
	map = (unsigned char**)malloc(y / 100 * sizeof(unsigned char *));
	for(int i = 0; i < y / 100; i++)
		*(map + i) = (unsigned char*)malloc(x / 100 * sizeof(unsigned char));

	for(int i = 0; i < y / 100; i++)
		for(int j = 0; j < x / 100; j++)
			map[i][j] = 0;

	initMap = 1;

	WORLD_X = x;
	WORLD_Y = y;

	RDV.x = WORLD_X / 2;
	RDV.y = WORLD_Y / 2;

	printf("[BOT] Map set to (%d, %d)\n", x, y);
}

void InitIA()
{
	nodes = NULL;

	initMap = 0;
	memset(saw_id, 0, 10 * sizeof(unsigned int));
	saw_i = 0;

	ticks = 0;

	iaStatus = EXPLORE;

	purple_status = GOTO;
	memset(&purple_communication_array, 0, sizeof(Vec2) * 20);
	purple_communication_target_id = 0;
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

		memcpy(node, pos, 18);

		if(node->flags & 0x8)
		{
			node->type = PLAYER;
			size_t nameLength = strlen(pos + NodeSize); //taille du nom
			node->name = malloc(nameLength+1); //on aloue la memoire pour le nom
			strcpy(node->name, data + startNodePos + (i+1)*NodeSize + totalNameLength); //on copie le nom
			totalNameLength += nameLength+1;//on augment la taille total des noms
		
			if(strcmp(node->name, BotName) == 0) //si la cellule est noter bot
			{
				player = node;
			}
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

	unsigned int new_pos = startNodePos + i*(NodeSize) + totalNameLength + sizeof(unsigned int); //nouvelle pos aprés avoir lu les cellules

	unsigned short nbDead; //nombre de cellule morte depuis la derniére fois
	memcpy(&nbDead, data + new_pos, sizeof(unsigned short)); //copie

	for(int j = 0; j < nbDead; j++) //pour chaque cellule morte
	{		
		unsigned int nodeID;
		memcpy(&nodeID, data + new_pos + sizeof(unsigned short) + j * sizeof(unsigned int), sizeof(unsigned int)); //on prend l'id
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

double distance(Vec2 a, Vec2 b)
{
	return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

Vec2 GetTarget(Node* brebie)
{
	Vec2 ret;

	Vec2 dest; dest.x = 0; dest.y = WORLD_Y / 2;
	double rb = distance(GetNodePos(brebie), dest);
	double ab = atan(brebie->y / brebie->x);

	ret.x = (rb + RAYON_BERGER + OFFSET)*cos(ab);
	ret.y = (rb + RAYON_BERGER + OFFSET)*sin(ab);

	return ret;
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

	unsigned int sorted_counter = 0;

	for(int i = 0; i < 20; i++)
	{
		if(!Vec2_isZero(purple_communication_array[i]) && !equalsVec2(purple_communication_array[i], RDV)
			&& !equalsVec2(purple_communication_array[i-1], purple_communication_array[i]))
		{
			sorted_path[sorted_counter++] = purple_communication_array[i];
		}
	}

	sorted_counter--;

	printf("[Bot-Purple] conteur = %d\n", sorted_counter);

	printf("[Bot-Purple] sorted Trajet viewer !\n");
	for(int i = 0; i < sorted_counter; i++)
		printf("(%d, %d) -> ", sorted_path[i].x, sorted_path[i].y);
	printf("\n-------------------------------------\n");

	Vec2 direction;
	direction.x = sorted_path[sorted_counter - 1].x - RDV.x;
	direction.y = sorted_path[sorted_counter - 1].y - RDV.y;

	printf("[Bot-Purple] Direction = (%d, %d) !\n", direction.x, direction.y);

	//direction.x *= 10;
	//direction.y *= 10;

	return direction;
}

void Berger(struct lws* wsi)
{
	if(player == NULL)
		return;

	Node* scout = NULL;
	Node* target = NULL;
	Vec2 direction;
	Node* brebie = NULL;

	switch(purple_status)
	{
	case GOTO:
		direction.x = 0; direction.y = 0;

		Move(wsi, RDV);

		if(equalsVec2(GetNodePos(player), RDV))
		{
			purple_status = LISTEN;
			printf("[Bot-Purple] Arrived at RDV !\n");
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
		printf("[BOT-Purple] info state, deltatime=%d\n", ticks - purple_ticks);
		target = NodeStack_get(nodes, purple_communication_target_id);
		if(ticks - purple_ticks > 20)
		{
			printf("[Bot-Purple] Info recevied : \n");
			show_path();

			direction = process_path();

			purple_status = BRING_BACK;
		}
		else if(target != NULL)
		{
			purple_communication_array[ticks - purple_ticks] = GetNodePos(target);
			debugNode(target);
		}

		break;

	case BRING_BACK:
		if(!Vec2_isZero(direction))
		{
			if((brebie = brebie_in_fov()) != NULL)
				Move(wsi, GetNodePos(brebie));
			else
				Move(wsi, direction);
		}
		else
		{
			printf("[Bot-Purple] Bug !\n");
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
