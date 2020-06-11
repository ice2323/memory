#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
bool movingForwards;
char direction;
static int currentHeadLocation;

float totalLatency = 0;
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
RequestNode* getNextRequest(){

	if(algorithm == 'F')
		return requestList -> front;
	else if(algorithm == 'T')
		return requestList -> front;
	else
		return requestList -> front;
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
			//increment variable here
		}
		movingForwards = goingForward;
		return directionChange;
	}
}
//time = distance/15 + (reverse_direction) ? 3 : 0;
void handleInput(){

	while(requestList -> size > 0){

		RequestNode* nextRequest = getNextRequest();
		bool directionChange;

		if(nextRequest -> arrivalTime > currentTime){

			distance = nextRequest -> location - currentHeadLocation;

			//currentTime = nextRequest -> arrivalTime;
			currentTime = distance / 15 + (directionChange) ? 3 : 0;
		}

		printf("%i\n", abs(distance));
		requestsProcessed++;
		currentHeadLocation = nextRequest -> location;
		dequeue(nextRequest);
	}
}
void output(){
	printf("Total amount of head movements required: %d\n", distanceTravelled);

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
