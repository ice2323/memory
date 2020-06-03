#include <stdio.h>
#include <stdlib.h>

int totalMemory; //total amount of memory in the system
int algorithm; //algorithm to be used. 0 for first fit, 1 for best fit, 2 for worst fit
int freeMemory; //amount of free memory available in the system
int freedMem = 0; //Sum of the amount of freed memory
int procTerminatedCount = 0; //count of the number of processes that were terminated
int allocatedMem = 0; //sum of the total allocated memory of the system
int procCreatedCount = 0; //count of the total number of processes created


/*
 * Nodes of the linked list represent memory frames
 */
typedef struct node {
    int val;
    int id; //process number, -1 if free memory
    int currentAddress; //address of memory location
    struct node * next;
    struct node * previous;
} node_t;
node_t * head = NULL; //pointer that points to the first node in the list

/*
 * Initializes list with one node whose value = total memory to represent total free memory in the system
 */
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

/*
 * This function traverses through the linked list printing all the nodes (apps and free memory locations)
 */
void printList(node_t * head) {
    node_t * ptr = head;
    while (ptr != NULL) {
    	if(ptr->id!=-1){
    		printf("Process %d size: %d bytes at address %d\n", ptr->id, ptr->val, ptr->currentAddress);
    	}else{
    		printf("Free memory of size %d bytes at address %d\n", ptr->val, ptr->currentAddress);
    	}
        ptr = ptr->next;
    }
}
/*
 * This function implements first fit algorithm to allocate memory to an application with ID app_id of size app_size
 N1 100
N2 400
N4 20
N3 5
T2
N6 800
N6 70
T1
N8 60
N9 400
N1 330
N2 100
N5 210
N90 30
N98 0
S
*/
void firstFit(int app_size, int app_id, node_t * ptr){
	int isHead = 0, isTail = 0;
	if(ptr==NULL){
		return;
	}
	while(ptr != NULL){

		if((ptr->val >= app_size) && (ptr->id == -1)){



			if(ptr->previous == NULL){ //node is head of the list
				isHead = 1;
			}
			if(ptr->next == NULL){ //node is tail of the list
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
									//printf("%d %d\n", app_size, ptr -> val);

			printf("Process %d of size %d bytes created successfully at address %d\n", app_id, app_size, newNode->currentAddress);
			if(ptr->val == 0){ //no more memory at that location, need to remove node
				printf("ZERO!\n");
				newNode->next = ptr->next;
				if(isTail==0){ //if not tail node
					ptr->next->previous = newNode;
				}
				free(ptr);
			}
			return;
		}
		ptr=ptr->next;
	}
	printf("NULL process %d failed to allocated %d bytes of memory\n", app_id, app_size);
}

/*
 * This function implements best fit algorithm to allocate memory to an application with ID app_id of size app_size
 */
void bestFit(int app_size, int app_id, node_t * ptr){
	if(ptr==NULL){
		return;
	}
	node_t * smallest = NULL;
	while(ptr != NULL){
		if((ptr->id == -1)&&(ptr->val >= app_size)){ //find first instance of free memory to compare with the rest of the nodes
			smallest = ptr;
			break;
		}
		ptr=ptr->next;
	}
	if (smallest==NULL){ //no free nodes or no free nodes that can accommodate the app size
		printf("NULL process %d failed to allocate %d bytes of memory\n", app_id, app_size);
		return;
	}
	int isHead = 0, isTail = 0;
	while(ptr != NULL){
		if((ptr->val >= app_size)&&(ptr->val < smallest->val)&&(ptr->id == -1)){// if node is free and of smaller size
			smallest = ptr;
		}
		ptr=ptr->next;
	}
	if(smallest->previous == NULL){ //node is head of the list
		isHead = 1;
	}
	if(smallest->next == NULL){ //node is tail of the list
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
	if(smallest->val == 0){ //no more memory at that location, need to remove node
		newNode->next = smallest->next;
		if(isTail==0){ //if not tail node
			smallest->next->previous = newNode;
		}
		free(smallest);
	}
	return;
}

/*
 * This function implements worst fit algorithm to allocate memory to an application with ID app_id of size app_size
 */
void worstFit(int app_size, int app_id, node_t * ptr){
	if(ptr==NULL){
		return;
	}
	node_t * largest = NULL;
	while(ptr != NULL){
		if((ptr->id == -1)&&(ptr->val >= app_size)){ //find first instance of free memory to compare with the rest of the nodes
			largest = ptr;
			break;
		}
		ptr=ptr->next;
	}
	if (largest==NULL){//no free nodes or no free nodes that can accommodate the app size
		printf("NULL process %d failed to allocate %d bytes of memory\n", app_id, app_size);
		return;
	}
	int isHead = 0, isTail = 0;
	while(ptr != NULL){
		if((ptr->val >= app_size)&&(ptr->val > largest->val)&&(ptr->id == -1)){ //if node is free and of larger size
			largest = ptr;
		}
		ptr=ptr->next;
	}
	if(largest->previous == NULL){ //node is head of the list
		isHead = 1;
	}
	if(largest->next == NULL){ //node is tail of the list
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
	if(largest->val == 0){ //no more memory at that location, need to remove node
		newNode->next = largest->next;
		if(isTail==0){ //if not tail node
			largest->next->previous = newNode;
		}
		free(largest);
	}
	return;
}

/*
 * This function merges nodes of free memory that are adjacent to node
 */
void mergeFree(node_t * node){
	node_t * temp = NULL;
	if((node->next != NULL)&&(node->previous != NULL)){ //neither head nor tail
		if((node->next->id == -1)&&(node->id == -1)){ //if the adjacent node to the front is also free, eliminate it
			temp = node->next;
			node->val += node->next->val; //merge the free space size
			if(temp->next != NULL){ //temp is not tail of the list
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
	}else if((node->next == NULL)&&(node->previous != NULL)){ //tail node
		if((node->previous->id == -1)&&(node->id == -1)){ //if node is free and previous node is free
			temp = node;
			node->previous->val += node->val;
			node->previous->next = node->next;
			if(node->next != NULL){
				node->next->previous = node->previous;
			}
			free(temp);
		}
	}else if((node->previous == NULL)&&(node->next != NULL)){
		if((node->next->id == -1)&&(node->id == -1)){ //if the adjacent node to the front is also free, eliminate it
			temp = node->next;
			node->val += node->next->val; //merge the free space size
			if(temp->next != NULL){ //temp is not tail of the list
				node->next->next->previous = node;
			}
			node->next = node->next->next;
			free(temp);
		}
	}

}

/*
 * This function terminates the application with the ID id
 */
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


int main(int argv, char**argc){
	if (argv != 4){
		printf("Please enter command line arguments correctly.");
		return 0;
	}
	//parsing total memory from input
	char c;
	sscanf(&argc[1][1], "%c", &c);
	if((c=='s') || (c=='S')){
		sscanf(&argc[2][0], "%i", &totalMemory);
	}else{
		printf("First command line argument must be total memory of the system.");
		return 0;
	}
	freeMemory = totalMemory;

	//parsing the algorithm type to be used
	sscanf(&argc[3][1], "%c", &c);
	if((c=='f')||(c=='F')){
		algorithm = 0;
	}else if((c=='b')||(c=='B')){
		algorithm = 1;
	}else if((c=='w')||(c=='W')){
		algorithm = 2;
	}else{
		printf("Incorrect algorithm type.");
		return 0;
	}

	head = init_list(totalMemory); //initializing list with the total free memory of the system

	//parsing the user input
	char input;
	int stop = 0;
	while(stop == 0){
		scanf("%c ", &input);
		int id, size;
		if(algorithm == 0){ //First fit
			if(input=='N'){
				scanf("%d", &id);
				scanf("%d", &size);
				firstFit(size, id, head);
			}else if(input == 'T'){
				scanf("%d", &id);
				terminate(id, head);
			}else if(input == 'S'){
				stop = 1;
			}
		}else if(algorithm == 1){ //Best fit
			if(input=='N'){
				scanf("%d", &id);
				scanf("%d", &size);
				bestFit(size, id, head);
			}else if(input == 'T'){
				scanf("%d", &id);
				terminate(id, head);
			}else if(input == 'S'){
				stop = 1;
			}
		}else if(algorithm == 2){ //Worst fit
			if(input=='N'){
				scanf("%d", &id);
				scanf("%d", &size);
				worstFit(size, id, head);
			}else if(input == 'T'){
				scanf("%d", &id);
				terminate(id, head);
			}else if(input == 'S'){
				stop = 1;
			}
		}
	}

	node_t * largestFragment = NULL;
	node_t * smallestFragment = NULL;
	node_t * ptr = head;
	while(ptr != NULL){ //find the first instance of a free block
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
	printf("\n------------------  Report  ------------------\n");
	printf("Total processes created = %d\n", procCreatedCount);
	printf("Total allocated memory = %d bytes\n", allocatedMem);
	printf("Total number of processes terminated = %d\n", procTerminatedCount);
	printf("Total freed memory = %d bytes\n", freedMem);
	printf("Final memory available = %d bytes\n", freeMemory);
	if(largestFragment!=NULL){ //if NULL there is no free memory block
		printf("Final largest fragmented memory %d bytes\n", largestFragment->val);
		printf("Final smallest fragmented memory %d bytes\n", smallestFragment->val);
	}else{
		printf("No fragmented memory blocks");
	}

	return 0;
}
