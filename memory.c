#include <stdio.h>
#include <stdlib.h>

int systemMemory;
int freedMemory;
int algorithm;
int processCreated;
int allocatedMemory;

typedef struct node{

	int val;
	int id;
	int currentAddress;
	struct node * next;
	struct node * previous;
} node_t;

node_t * head = NULL;

node_t * init_list(int val){

	node_t * newNode = malloc(sizeof(node_t));
	if (newNode == NULL) {
	    return NULL;
	}
	newNode->next = NULL;
	newNode->previous = NULL;
	newNode->val = val;
	newNode->id = -1;
	newNode->currentAddress = 0;
	return newNode;
}

void firstFit(int psize, int pid, node_t *ptr){

	int isHead = 0, isTail = 0;

	if(ptr == NULL) return;

	while(ptr != NULL){

		if((ptr -> val >= psize) && (ptr -> id == -1)){

			if(ptr -> previous == NULL) { isHead = 1; }

			if(ptr -> next = NULL){ isTail = 1; }

			node_t *newNode = malloc(sizeof(node_t));

			if(isHead == 1){ head = newNode; }

			newNode -> currentAddress = ptr -> currentAddress;
			newNode -> id = pid;
			newNode -> val = psize;
			newNode -> next = ptr;
			newNode -> previous = ptr -> previous;
			ptr -> val -= psize;
			ptr -> currentAddress += psize;

			if(isHead == 0){

				ptr -> previous -> next = newNode;
			}

			ptr -> previous = newNode;

			freedMemory -= psize;
			allocatedMemory += psize;
			processCreated++;
			printf("Process %d of size %d bytes created successfully at address %d\n", pid, psize, newNode->currentAddress);

			if(ptr -> val == 0){

				newNode -> next = ptr -> next;

				if(isTail == 0){

					ptr -> next -> previous = newNode;
				}
				free(ptr);
			}
			return;
		}
		ptr = ptr -> next;
	}
	printf("%s %d %s %d %s", "Process #", pid, "failed to allocate", psize, "memory\n");
}

int main(int argc, char *argv[]){

	if(argc != 4){
		printf("%s\n", "Not enough command line arguments!");
		return EXIT_FAILURE;
	}
	char c;
	c = *argv[2];
	printf("%c\n", c);
	//sscanf(&argc[1][1], "%c", &c);
	if((c == 's') || (c == 'S')){
		systemMemory = atoi(argv[2]);
		//sscanf(&argc[2][0], "%i", &systemMemory);
	}else{
		printf("%s\n", "Total memory not specified!");
		return EXIT_FAILURE;
	}
	freedMemory = systemMemory;

	//sscanf(&argc[3][1], "%c", &c);
	c = atoi(argv[3]);

	if((c=='f')||(c=='F')){
		algorithm = 0;
	}else if((c=='b')||(c=='B')){
		algorithm = 1;
	}else if((c=='w')||(c=='W')){
		algorithm = 2;
	}else{
		printf("Incorrect algorithm type.");
		return EXIT_FAILURE;
	}

	head = init_list(systemMemory);

	char input;
	int x;
	//algorithm = 0;
	int id, size;

	while(x != 10){

		scanf(" %c", &input);
		switch(algorithm){

			case 0:

				switch(input){

					case 'S':
						return 0;

					case 'N':
						printf("%s\n", "case N");
						scanf("%d", &id);
						scanf("%d", &size);
						firstFit(size, id, head);
						break;

					case 'T':
						printf("%s\n", "case T");
				}
				break;
			case 1:

				switch(input){

					case 'S':
						return 0;

					case 'N':
						printf("%s\n", "case 1 N");
						scanf("%d", &id);
						scanf("%d", &size);
						break;

					case 'T':
						printf("%s\n", "case 1 T");
				}

				break;
			case 2:

				switch(input){

					case 'S':
						return 0;

					case 'N':
						printf("%s\n", "case 2 N");
						scanf("%d", &id);
						scanf("%d", &size);
						break;

					case 'T':
						printf("%s\n", "case 2 T");
				}
				break;
			default:
				printf("%s\n", "incorrect case!");
				x = 10;

		}


	
	}
}
