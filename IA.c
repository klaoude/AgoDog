#include "IA.h"

void InitIA()
{
	nodes = NULL;
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
		}
		else if(node->flags&0x1)
			node->type = VIRUS;
		else
			node->type = FOOD;
		
		NodeStack_update(&nodes, node);

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

	printNodeStack(nodes);
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
		/*printf("Game area size\n");
		double a,b,c,d;
		memcpy(&a, payload+1, 8);
		memcpy(&b, payload+1+8, 8);
		memcpy(&c, payload+1+2*8, 8);
		memcpy(&d, payload+1+3*8, 8);
		printf("(%f, %f, %f, %f)\n", a, b, c, d);*/
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
