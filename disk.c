#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define FREE_BLOCK -2
#define END_OF_FILE -1

typedef struct BlockNode{
	int index;
	int nextIndex;
	bool accountFor;
	struct BlockNode* next;
} BlockNode;

typedef struct BlockList{
	BlockNode* front;
	int size;
} BlockList;

BlockList* blockList;

typedef struct FileNode{
	int fileLength;
	int startingIndex;
	BlockNode* startingNode;
	struct FileNode* next;
} FileNode;

typedef struct FileList{
	FileNode* front;
	int size;
} FileList;

FileList* fileList;

static int nextFreeSpotInMemory = 0;
static int numberOfFreeBlocksToAdd = 0;
static int totalNumberOfBlocksMoved = 0;



void insertIntoSortedFileList(FileNode* file) {
    // files are sorted by starting index to allow for easier printing
    fileList->size++;
    if (fileList->front == NULL) {
        fileList->front = file;
    } else if (file->startingIndex < fileList->front->startingIndex) { 
        // front of list
        file->next = fileList->front;
        fileList->front = file;
    } else { 
        // somewhere else in the list
        FileNode* pointer = fileList->front;
        while (pointer->next != NULL) {
            if (file->startingIndex < pointer->next->startingIndex) {
                file->next = pointer->next;
                pointer->next = file;
                return;
            }
            pointer = pointer->next;
        }
        pointer->next = file;
    }
}

FileNode* createFileNode(int fileLength, int startingIndex, BlockNode* startingNode) {
    FileNode* file = (FileNode*) malloc(sizeof(FileNode));
    file->fileLength = fileLength;
    file->startingIndex = startingIndex;
    file->startingNode = startingNode;
    file->next = NULL;
    return file;
}

BlockNode* createBlockNode(int index, int nextIndex){

	BlockNode * block = (BlockNode*) malloc(sizeof(BlockNode));
	block -> accountFor = false;
	block -> index = index;
	block -> next = NULL;
	block -> nextIndex = nextIndex;
	return block;
}

void appendToBlockList(BlockNode* block){

	if(blockList -> front == NULL){
		blockList -> front = block;
	}else{
		BlockNode* pointer = blockList -> front;

		while(pointer -> next != NULL){
			pointer = pointer -> next;
		}
		pointer -> next = block;
	}
	blockList -> size++;
}

void removeFromBlockList(){

	if(blockList -> front == NULL){
		return;
	}
	blockList -> front = blockList -> front -> next;
	blockList -> size--;
}
void initializeFileList(){

	fileList = (FileList*) malloc(sizeof(FileList));
	fileList -> front = NULL;
	fileList -> size = 0;
}

void initializeBlockList(){
	blockList = (BlockList*) malloc(sizeof(BlockList));
	blockList -> front = NULL;
	blockList -> size = 0;
}

void readInput(){

	initializeBlockList();

	int block, next;

	while(scanf("%d %d", &block, &next) > 1){

		BlockNode* blockNode = createBlockNode(block, next);
		appendToBlockList(blockNode);
		scanf("\n");
	}
}
BlockNode* findBlockBeforeNode(BlockNode* blockNode){

	BlockNode* pointer = blockList -> front;

	while(pointer != NULL){

		if(pointer -> nextIndex == blockNode -> index){
			return pointer;
		}
		pointer = pointer -> next;
	}
	return NULL;
}

void buildFileFromEnd(BlockNode* end, int filesize){

	end -> accountFor = true;
	BlockNode* prevBlock = findBlockBeforeNode(end);

	if(prevBlock == NULL){

		FileNode* fileNode = createFileNode(filesize, end -> index, end);
		insertIntoSortedFileList(fileNode);
	}else{
		buildFileFromEnd(prevBlock, filesize + 1);
	}
}

BlockNode* findBlockAfterNode(BlockNode* blockNode) {
    BlockNode* pointer = blockList->front;
    while (pointer != NULL) {
        if (blockNode->nextIndex == pointer->index) {
            return pointer;
        }
        pointer = pointer->next;
    }
    return NULL;
}

void processInput(){

	initializeFileList();

	BlockNode * pointer = blockList -> front;

	while(pointer != NULL){

		if(pointer -> accountFor){

			pointer = pointer -> next;
			continue;
		}
		if(pointer -> nextIndex == FREE_BLOCK){

			numberOfFreeBlocksToAdd++;
			pointer -> accountFor = true;
		}else if(pointer -> nextIndex == END_OF_FILE){

			buildFileFromEnd(pointer, 1);
		}
		pointer = pointer -> next;
	}
}
void updateNumberOfBlocksMoved(FileNode* file, int newIndex) {
    BlockNode* pointer = file->startingNode;
    int i = 0;
    while (pointer != NULL) {
        if (pointer->index != newIndex + i) {
            totalNumberOfBlocksMoved++;
        }
        pointer = findBlockAfterNode(pointer);
        i++;
    }
}
int main(){

	readInput();

		printf("%s\n", "dwlmkmk");

	processInput();


	int outputArray[blockList -> size];

	FileNode* pointer = fileList -> front;

	int i;

	while(pointer != NULL){

		for(i = 0; i < pointer -> fileLength - 1; i++){

			outputArray[nextFreeSpotInMemory + i] = nextFreeSpotInMemory + i + 1;
		}
		outputArray[nextFreeSpotInMemory + i] = END_OF_FILE;
		updateNumberOfBlocksMoved(pointer, nextFreeSpotInMemory);
		nextFreeSpotInMemory += pointer -> fileLength;
		pointer = pointer -> next;
	}
	for(int j = blockList -> size - 1; j > blockList -> size -1 - numberOfFreeBlocksToAdd; j++){
		outputArray[j] = FREE_BLOCK;
	}
	printf("%s %d\n", "Total number of blocks moved:", totalNumberOfBlocksMoved);
	printf("%s\n", "New List Layout is:");

	for(int x = 0; x < blockList -> size; x++){

		printf("%d %d\n", x, outputArray[x]);
	}
}
