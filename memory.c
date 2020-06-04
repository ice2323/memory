#include <stdio.h>
#include <stdlib.h>

int totalMemory; 
int algorithm; 
int freeMemory;
int freedMem = 0; 
int procTerminatedCount = 0; 
int allocatedMem = 0; 
int procCreatedCount = 0; 


typedef struct node {
    int val;
    int id; //process number, -1 if free memory
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
void firstFit(int app_size, int app_id, node_t * ptr){
	int isHead = 0, isTail = 0;
	if(ptr==NULL){
		return;
	}
	while(ptr != NULL){

		if((ptr->val >= app_size) && (ptr->id == -1)){

			if(ptr->previous == NULL){ 
				isHead = 1;
			}
			if(ptr->next == NULL){ 
				isTail = 1;
			}
			node_t *newNode = malloc(sizeof(node_t));
			if(isHead==1){
				head = newNode;
			}
			newNode->currentAddress = ptr->currentAddress;
			newNode->id = app_id;
			newNode->val = app_size;
			newNode->next = ptr;
			newNode->previous = ptr->previous;
			ptr->val -= app_size;
			ptr->currentAddress += app_size;

			if(isHead==0){
				ptr->previous->next = newNode;
			}

			ptr->previous = newNode;
			freeMemory -= app_size;
			allocatedMem += app_size;
			procCreatedCount++;
			printf("%s %d %s %d %s %d\n", "Process", app_id, "of size", app_size, "created successfully at address", newNode -> currentAddress);
			return;

		}
		ptr=ptr->next;
	}
	printf("%s %d %s %d %s\n", "Process", app_id, "failed to allocate", app_size, "bytes");
}

void bestFit(int app_size, int app_id, node_t * ptr){
	if(ptr==NULL){
		return;
	}
	node_t * smallest = NULL;
	while(ptr != NULL){
		if((ptr->id == -1)&&(ptr->val >= app_size)){ 
			smallest = ptr;
			break;
		}
		ptr=ptr->next;
	}
	if (smallest==NULL){ 
		printf("%s %d %s %d %s\n", "Process", app_id, "failed to allocate", app_size, "bytes");
		return;
	}
	int isHead = 0, isTail = 0;
	while(ptr != NULL){
		if((ptr->val >= app_size)&&(ptr->val < smallest->val)&&(ptr->id == -1)){
			smallest = ptr;
		}
		ptr=ptr->next;
	}
	if(smallest->previous == NULL){ 
		isHead = 1;
	}
	if(smallest->next == NULL){
		isTail = 1;
	}
	node_t *newNode = malloc(sizeof(node_t));
	newNode->currentAddress = smallest->currentAddress;
	newNode->id = app_id;
	newNode->val = app_size;
	newNode->next = smallest;
	newNode->previous = smallest->previous;
	smallest->val -= app_size;
	smallest->currentAddress += app_size;
	if(isHead==0){
		smallest->previous->next = newNode;
	}
	smallest->previous = newNode;
	if(isHead==1){
		head = newNode;
	}
	freeMemory -= app_size;
	allocatedMem += app_size;
	procCreatedCount++;
	printf("Process %d of size %d bytes created successfully at address %d\n", app_id, app_size, newNode->currentAddress);
	if(smallest->val == 0){ 
		newNode->next = smallest->next;
		if(isTail==0){ 
			smallest->next->previous = newNode;
		}
		free(smallest);
	}
	return;
}

void worstFit(int app_size, int app_id, node_t * ptr){
	if(ptr==NULL){
		return;
	}
	node_t * largest = NULL;
	while(ptr != NULL){
		if((ptr->id == -1)&&(ptr->val >= app_size)){ 
			largest = ptr;
			break;
		}
		ptr=ptr->next;
	}
	if (largest==NULL){
		printf("NULL process %d failed to allocate %d bytes of memory\n", app_id, app_size);
		return;
	}
	int isHead = 0, isTail = 0;
	while(ptr != NULL){
		if((ptr->val >= app_size)&&(ptr->val > largest->val)&&(ptr->id == -1)){ 
			largest = ptr;
		}
		ptr=ptr->next;
	}
	if(largest->previous == NULL){ 
		isHead = 1;
	}
	if(largest->next == NULL){
		isTail = 1;
	}
	node_t *newNode = malloc(sizeof(node_t));
	newNode->currentAddress = largest->currentAddress;
	newNode->id = app_id;
	newNode->val = app_size;
	newNode->next = largest;
	newNode->previous = largest->previous;
	largest->val -= app_size;
	largest->currentAddress += app_size;
	if(isHead==0){
		largest->previous->next = newNode;
	}
	largest->previous = newNode;
	if(isHead==1){
		head = newNode;
	}
	freeMemory -= app_size;
	allocatedMem += app_size;
	procCreatedCount++;
	printf("Process %d of size %d bytes created successfully at address %d\n", app_id, app_size, newNode->currentAddress);
	if(largest->val == 0){ 
		newNode->next = largest->next;
		if(isTail==0){ 
			largest->next->previous = newNode;
		}
		free(largest);
	}
	return;
}

void mergeFree(node_t * node){
	node_t * temp = NULL;
	if((node->next != NULL)&&(node->previous != NULL)){ 
		if((node->next->id == -1)&&(node->id == -1)){ 
			temp = node->next;
			node->val += node->next->val; 
			if(temp->next != NULL){ 
				node->next->next->previous = node;
			}
			node->next = node->next->next;
			free(temp);
		}else if((node->previous->id == -1)&&(node->id == -1)){
			temp = node;
			node->previous->val += node->val;
			node->previous->next = node->next;
			if(node->next != NULL){
				node->next->previous = node->previous;
			}
			free(temp);
		}
	}else if((node->next == NULL)&&(node->previous != NULL)){ 
		if((node->previous->id == -1)&&(node->id == -1)){ 
			temp = node;
			node->previous->val += node->val;
			node->previous->next = node->next;
			if(node->next != NULL){
				node->next->previous = node->previous;
			}
			free(temp);
		}
	}else if((node->previous == NULL)&&(node->next != NULL)){
		if((node->next->id == -1)&&(node->id == -1)){ 
			temp = node->next;
			node->val += node->next->val; 
			if(temp->next != NULL){ 
				node->next->next->previous = node;
			}
			node->next = node->next->next;
			free(temp);
		}
	}

}
node_t * largestFragment = NULL;
node_t * smallestFragment = NULL;

void terminate(int id, node_t * ptr){
	while(ptr != NULL){
		if(ptr->id == id){
			ptr->id = -1;
			freeMemory+=ptr->val;
			freedMem += ptr->val;
			procTerminatedCount++;
			printf("Process %d terminated successfully\n", id);
			mergeFree(ptr);
			mergeFree(ptr);
			return;
		}
		ptr = ptr->next;
	}
	printf("Process %d failed to free memory.\n", id);
}
void printReport(){

	printf("Total processes created = %d\n", procCreatedCount);
	printf("Total allocated memory = %d bytes\n", allocatedMem);
	printf("Total number of processes terminated = %d\n", procTerminatedCount);
	printf("Total freed memory = %d bytes\n", freedMem);
	printf("Final memory available = %d bytes\n", freeMemory);
	if(largestFragment!=NULL){ //if NULL there is no free memory block
		printf("Final largest fragmented memory %d bytes\n", largestFragment->val);
		printf("Final smallest fragmented memory %d bytes\n", smallestFragment->val);
	}else{
		printf("No fragmented memory blocks\n");
	}
}

int main(int argc, const char * argv[]){

	if (argc != 4){
		printf("Please enter command line arguments correctly.");
		return EXIT_FAILURE;
	}

	char c;
	totalMemory = atoi(argv[2]);

	if(totalMemory < 0){
		printf("Please provide some memory!\n");
		return EXIT_FAILURE;
	}
	freeMemory = totalMemory;

	c = *(argv[3] + 1);
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

	head = init_list(totalMemory); 
	char input;
	int stop = 0;
	while(stop == 0){

		scanf("%c ", &input);
		int id, size;

		switch(algorithm){

			case 0:		//first fit

				switch(input){

					case 'N':
						scanf("%d", &id);
						scanf("%d", &size);
						firstFit(size, id, head);
						break;
					case 'T':
						scanf("%d", &id);
						terminate(id, head);
						break;
					case 'S':
						stop = 1;
				}
				break;
			case 1:		//best fit

				switch(input){

					case 'N':
						scanf("%d", &id);
						scanf("%d", &size);
						bestFit(size, id, head);
						break;
					case 'T':
						scanf("%d", &id);
						terminate(id, head);
						break;
					case 'S':
						stop = 1;
				}
				break;
			case 2:		//worst fit

				switch(input){

					case 'N':
						scanf("%d", &id);
						scanf("%d", &size);
						worstFit(size, id, head);
						break;
					case 'T':
						scanf("%d", &id);
						terminate(id, head);
						break;
					case 'S':
						stop = 1;
				}
				break;
		}
	}
	
	node_t * ptr = head;
	while(ptr != NULL){ 
		if(ptr->id==-1){
			largestFragment = smallestFragment = ptr;
			break;
		}
		ptr = ptr->next;
	}
	while(ptr != NULL){
		if((ptr->id == -1)&&(ptr->val > largestFragment->val)){
			largestFragment = ptr;
		}
		if((ptr->id == -1)&&(ptr->val < smallestFragment->val)){
			smallestFragment = ptr;
		}
		ptr = ptr->next;
	}
	printf("\n====================  Report  ====================\n");
	printReport();
	printf("==================================================\n");

	return EXIT_SUCCESS;
}
