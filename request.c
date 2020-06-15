#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

typedef struct RequestNode{
	int location;
	int arrivalTime;
	struct RequestNode *next;
	struct RequestNode *prev;
} RequestNode;

typedef struct RequestList{
	int size;
	RequestNode *front;
	RequestNode *end;
} RequestList;

RequestList *requestList;

static unsigned int currentTime;
char algorithm;
bool movingForwards;			//false (0)
char direction;
static int currentHeadLocation;

float totalLatency = 0;
int directionChanges = 0;
int requestsProcessed = 0;
int distanceTravelled = 0;
int distance = 0;

void initializeRequestList() {
    requestList = (RequestList*) malloc(sizeof(RequestList));
    requestList->end = NULL;
    requestList->front = NULL;
    requestList->size = 0;
}

RequestNode * createNewRequest(int location, int arrivalTime){

	RequestNode* node = (RequestNode*) malloc(sizeof(RequestNode));
	node -> arrivalTime = arrivalTime;
	node -> location = location;
	node -> next = NULL;
	node -> prev = NULL;
	return node;
}

void enqueue(RequestNode *request){

	if(requestList -> front == NULL){
		requestList -> front = request;
		requestList -> end = request;
	}else{
		requestList -> end -> next = request;
		request -> prev = requestList -> end;
		requestList -> end = request;
	}
	requestList -> size++;
}

void buildRequestList(){
	initializeRequestList();

	int sectorNum, arrivalTime;

	while(scanf("%d %d", &sectorNum, &arrivalTime) > 1){

		if(sectorNum < 0 || sectorNum > 99999){

			printf("%s\n", "Invalid head location!");
			return;
		}
		RequestNode *request = createNewRequest(sectorNum, arrivalTime);
		enqueue(request);
		scanf("\n");
	}
}


void dequeue(RequestNode* request) {
    if (requestList->size == 1) {
        requestList->front = NULL;
        requestList->end = NULL;
    } else if (requestList->front == request) { // start of requestList
        requestList->front = requestList->front->next;
        requestList->front->prev = NULL;
    } else if (requestList->end == request) { // end of requestList
        requestList->end->prev->next = NULL;
        requestList->end = requestList->end->prev;
    } else { // somewhere else in the requestList
        RequestNode* pointer = requestList->front->next;
        while (pointer != NULL) {
            if (pointer == request) {
                pointer->prev->next = pointer->next;
                pointer->next->prev = pointer->prev;
                break;
            }
            pointer = pointer->next;
        }
    }
    requestList->size--;
    free(request);
}

int distanceFromHead(RequestNode* request){
	int distanceFromHead = request -> location - currentHeadLocation;
	return distanceFromHead < 0 ? distanceFromHead * -1 : distanceFromHead;
}

RequestNode* findFirstSSTF(){

	RequestNode* first = requestList -> front;
	RequestNode* pointer = requestList -> front;

	while(pointer != NULL && pointer -> arrivalTime == first -> arrivalTime){

		if(distanceFromHead(pointer) < distanceFromHead(first)){

			first = pointer;
		}
		pointer = pointer -> next;
	}
	return first;
}
RequestNode * findNextSSTF(){

	if(currentTime == 0){
		return findFirstSSTF();
	}else{
		RequestNode* next = requestList -> front;
		RequestNode* pointer = requestList -> front;

		while(pointer != NULL && pointer -> arrivalTime <= currentTime){

			if(distanceFromHead(pointer) < distanceFromHead(next)){
				next = pointer;
			}
			pointer = pointer -> next;
		}
		return next;
	}
}
int distanceFromHeadCSCAN(RequestNode* request){

	int distanceAway;

	if(movingForwards && request -> location < currentHeadLocation){

		distanceAway = ((99999 - currentHeadLocation) + 1) + request -> location;
	}else if(!movingForwards && request -> location > currentHeadLocation){

		distanceAway = (currentHeadLocation) + 1 + (99999 - request -> location);
	}else{

		distanceAway = distanceFromHead(request);
	}
	return distanceAway;
}
RequestNode * findNextCSCAN(){

	if(currentTime == 0){

		RequestNode * first = findFirstSSTF();
		movingForwards = first -> location >= currentHeadLocation;
		return first;
	}else{
		RequestNode* next = requestList -> front;
		RequestNode * pointer = requestList -> front;

		while(pointer != NULL && pointer -> arrivalTime <= currentTime){

			if(distanceFromHeadCSCAN(pointer) < distanceFromHeadCSCAN(next)){

				next = pointer;
			}
			pointer = pointer -> next;
		}
		return next;
	}
}
RequestNode* getNextRequest(){

	if(algorithm == 'F'){
		return requestList -> front;
	}else if(algorithm == 'T'){
		return findNextSSTF();
	}else{
		return findNextCSCAN();
	}

}
//returns true if direction changes, false otherwise 
bool directionChanged(RequestNode* nextRequest){

	if(currentTime == 0){

		movingForwards = nextRequest -> location >= currentHeadLocation;
		return false;
	}else if(algorithm == 'C'){
		return false;
	}else{
		bool goingForward = nextRequest -> location >= currentHeadLocation;
		bool directionChange = movingForwards != goingForward;

		if(directionChange){
			directionChanges++;
		}
		movingForwards = goingForward;
		return directionChange;
	}
}

bool changeDirectionForwards(RequestNode* nextRequest){

	if(nextRequest -> location > currentHeadLocation){
		return false;
	}else{
		directionChanges++;

		return true;
	}
}
bool changeDirectionBackwards(RequestNode* nextRequest){

	if(nextRequest -> location < currentHeadLocation){
		return false;
	}else{
		directionChanges++;

		return true;
	}
}
//time = distance/15 + (reverse_direction) ? 3 : 0;
void handleInput(){

	int timeToComplete = 0;

	bool isStart = true;
	bool startCheck = false;
	bool changeAtStart;
	bool directionChange;

	//RequestNode* start = getNextRequest();

	
	while(requestList -> size > 0){

		RequestNode* nextRequest = getNextRequest();
		
		//check if we need to change the direction

		if(algorithm == 'F'){
			

			if(!startCheck){

				startCheck = true;
				if(direction == 'A'){
					changeDirectionForwards(nextRequest);
				}
				if(direction == 'D'){
					changeDirectionBackwards(nextRequest);
				}
			}
			directionChange = directionChanged(nextRequest);
		
			//	printf(directionChange ? "true\n" : "false\n");
			distance = nextRequest -> location - currentHeadLocation;
			distanceTravelled += abs(distance);
			if(nextRequest -> arrivalTime > currentTime){

				currentTime = nextRequest -> arrivalTime;
			}

			currentTime = abs(distance / 15);

			if(directionChange){
				currentTime = currentTime + 3;
			}
			//printf("%d\n", abs(currentTime));
			
			requestsProcessed++;
			currentHeadLocation = nextRequest -> location;
			dequeue(nextRequest);
		}else if(algorithm == 'T'){

			bool directionChange = directionChanged(nextRequest);

			if(nextRequest -> arrivalTime > currentTime){
				currentTime = nextRequest -> arrivalTime;
			}
			distance = distanceFromHead(nextRequest);
			distanceTravelled += distance;
			printf("%s %d\n", "distance:", distanceTravelled);
			printf("%s %d\n", "Currently processing:",  nextRequest -> location);

			currentTime = currentTime + 15;
			requestsProcessed++;
			currentHeadLocation = nextRequest -> location;
			dequeue(nextRequest);
		}else if(algorithm == 'C'){

			int currentMinDistance = INT_MAX;

			RequestNode* node = requestList -> front;

			if(direction == 'A'){
				fflush(stdout);
				fflush(stdin);
				printf("%d\n", node -> location);
				fflush(stdout);
				fflush(stdin);

				while(node -> next != NULL){

					if(abs(node -> location - node -> next -> location) < currentMinDistance){

						currentMinDistance = node -> location;
					}
					node = node -> next;
				}
			}
			dequeue(node);
		}
	}
}
void output(){
	printf("Total amount of head movements required: %d\n", distanceTravelled);
	printf("%s %d\n", "Total jobs processed:", requestsProcessed);
	printf("%s %d\n", "Total direction changes:", directionChanges);
}

int main(int argc, const char * argv[]){

	if(argc < 3){
		printf("%s\n", "Please provide command line args!");
		return EXIT_FAILURE;
	}
	currentHeadLocation = atoi(argv[2]);

	if(currentHeadLocation < 0 || currentHeadLocation > 9999){
		printf("%s\n", "Invalid head location");
		return EXIT_FAILURE;
	}
	//the algorithm to use
	algorithm = argv[1][0];

	if(algorithm != 'F' && algorithm != 'f' && algorithm != 'T' && algorithm != 't' && algorithm != 'C' && algorithm != 'c'){
		return EXIT_FAILURE;
		printf("%s\n", "Please provide algorithm as F, T, or C!!");
	}
	//which direction the disk is moving at start
	direction = argv[3][0];

	if(direction != 'a' && direction != 'd' && direction != 'A' && direction != 'D'){
		printf("%s\n", "Please provide direction as A or D!");
		return EXIT_FAILURE;
	}
	buildRequestList();
	handleInput();
	output();
}
