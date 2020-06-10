#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct RequestNode{
	int location;
	int arrivalTime;
	struct RequestNode *next;
	struct RequestNode *prev;
} RequestNode;

RequestNode* createNewRequest(int location, int arrivalTime);

typedef struct RequestList{
	int size;
	RequestNode *front;
	RequestNode *end;
} RequestList;

RequestList *requestList;

void initializeRequestList();

static unsigned int currentTime = 0;
static int currentHeadLocation;
char algorithm;
bool movingForwards;

float totalLatency = 0;
int totalRequestsProcessed = 0;
int totalDirectionChanges = 0;
int totalDistanceTravelled = 0;

void buildRequestList();

int main(int argc, const char* argv[]){

	currentHeadLocation = atoi(argv[2]);

	if(currentHeadLocation < 0 || currentHeadLocation > 99999){
		printf("%s\n", "Error: Invalid head location!");
		return EXIT_FAILURE;
	}
	algorithm = argv[1][0];
}
void initializeRequestList(){
	requestList = (RequestList*) malloc(sizeof(RequestList));
	requestList -> end = NULL;
	requestList -> front = NULL;
	requestList -> size = 0;
}
void buildRequestList(){

	initializeRequestList();
	int sectorNum, arrivalTime;

	while(scanf("%d %d", &sectorNum, &arrivalTime) > 1){

		if(sectorNum < 0 || sectorNum > 99999){

			printf("%s\n", "Invalid request!");
			return EXIT_FAILURE;
		}
		RequestNode *request = createNewRequest(sectorNum, arrivalTime);
		appendRequestToList(request);
		scanf("\n");
	}
}
//assignment nine
