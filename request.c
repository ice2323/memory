#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

int counter = 0;

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
int start = 0;

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

		//check if the sector number is outside the range of the disk
		if(sectorNum < 0 || sectorNum > 99999){

			printf("%s\n", "Invalid head location!");
			return;
		}
		counter++;
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

RequestNode* findFirstSSTFNegative(){

	RequestNode* first = requestList -> front;
	RequestNode* pointer = requestList -> front;

	while(pointer != NULL && pointer -> arrivalTime == first -> arrivalTime){

		if(distanceFromHead(pointer) > distanceFromHead(first)){

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

	if(movingForwards && request -> location < currentHeadLocation){		// <<<<<<<<

		distanceAway = ((99999 - currentHeadLocation) + 1) + request -> location;
	}else if(!movingForwards && request -> location  > currentHeadLocation){	// >>>>>>>>>

		distanceAway = (currentHeadLocation) + 1 + (99999 - request -> location);
	}else{

		distanceAway = distanceFromHead(request);
	}
	return distanceAway;
}
//function calculates and returns the amount of time it takes the current request
//to complete based on if the direction changes.
float calculateTimeToComplete(RequestNode* request, bool reverseDirection) {
    int distance;
    if (algorithm == 'C') {

    	if(direction == 'A'){
        	distance = distanceFromHeadCSCAN(request);
    	}else{
    		distance = distanceFromHeadCSCAN(request);
    	}
    } else {
        distance = distanceFromHead(request);
    }
    distanceTravelled += distance;
    float timeToComplete = (float) distance / (float) 15;
    timeToComplete = reverseDirection ? timeToComplete + 3 : timeToComplete;
    //printf("%s %f\n", "time to complete:", timeToComplete);
    float waitTime = currentTime - request->arrivalTime;
    totalLatency += timeToComplete + waitTime;
    return timeToComplete;
}
RequestNode * findNextCSCAN(){

	if(currentTime == 0){
		//important part
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
RequestNode * findNextcscanNegative(){

	if(currentTime == 0){

		RequestNode * first = findFirstSSTFNegative();
		movingForwards = first -> location <= currentHeadLocation;
		return first;
	}else{
		RequestNode* next = requestList -> front;
		RequestNode * pointer = requestList -> front;

		while(pointer != NULL && pointer -> arrivalTime <= currentTime){

			if(distanceFromHeadCSCAN(pointer) > distanceFromHeadCSCAN(next)){

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

		if(direction == 'A')
			return findNextCSCAN();
		else{
			return findNextcscanNegative();
		}
	}

}
//returns true if direction changes, false otherwise 
bool directionChanged(RequestNode* nextRequest){

	if(currentTime == 0){

		movingForwards = nextRequest -> location >= currentHeadLocation;
		return false;
	}else if(algorithm == 'C'){
		//no direction changes for cscan
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
//these functions check if we need to change directions at the start
//based on the initial direction the disk is spining
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
	
			if(nextRequest -> arrivalTime > currentTime){

				currentTime = nextRequest -> arrivalTime;
			}
			currentTime += calculateTimeToComplete(nextRequest, directionChange);
			requestsProcessed++;
			currentHeadLocation = nextRequest -> location;
			dequeue(nextRequest);
		}else if(algorithm == 'T'){

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

			if(nextRequest -> arrivalTime > currentTime){
				currentTime = nextRequest -> arrivalTime;
			}
			currentTime += calculateTimeToComplete(nextRequest, directionChange);

			requestsProcessed++;
			currentHeadLocation = nextRequest -> location;
			dequeue(nextRequest);
			
		}else if(algorithm == 'C'){

			if(direction == 'A'){
				directionChange = directionChanged(nextRequest);

				if(nextRequest -> arrivalTime > currentTime){
					currentTime = nextRequest -> arrivalTime;
				}
				printf("%s %d\n", "Currently processing:",  nextRequest -> location);

				currentTime += calculateTimeToComplete(nextRequest, directionChange);
				requestsProcessed++;
				currentHeadLocation = nextRequest -> location;
				dequeue(nextRequest);
			}else{
				//cscan going downwards
				directionChange = directionChanged(nextRequest);

				if(nextRequest -> arrivalTime > currentTime){
					currentTime = nextRequest -> arrivalTime;
				}
				printf("%s %d\n", "Currently processing:",  nextRequest -> location);

				currentTime += calculateTimeToComplete(nextRequest, directionChange);
				requestsProcessed++;
				currentHeadLocation = nextRequest -> arrivalTime;
				dequeue(nextRequest);
			}		
		}
	}
}
void output(){
	printf("Total amount of head movements required: %d\n", distanceTravelled);
	printf("%s %d\n", "Total time:", currentTime);
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
	direction = argv[3][0];

	if(algorithm != 'F' && algorithm != 'f' && algorithm != 'T' && algorithm != 't' && algorithm != 'C' && algorithm != 'c'){
		return EXIT_FAILURE;
		printf("%s\n", "Please provide algorithm as F, T, or C!!");
	}

	if(direction != 'a' && direction != 'd' && direction != 'A' && direction != 'D'){
		printf("%s\n", "Please provide direction as A or D!");
		return EXIT_FAILURE;
	}
	
	buildRequestList();
	handleInput();
	output();

	free(requestList);

	return EXIT_SUCCESS;
}
