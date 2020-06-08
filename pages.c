#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#define SIZE (int)pow(2,12)
int number_frames;
long pageMask = 0x3FF; //0xFFFFF;
// 0x3FF;		//20 bits
int offsetMask = 0xFFF;		//12 bits
int pageFault = 0;
int dirtyPageCount = 0;
int globalAge = 0;

typedef struct pageTableEntry{
	int invalid;
	int frame;
} ptEntry;

typedef struct physicalMemoryEntry{
	int empty;
	int clean;
	int age;
	unsigned long int value;
} pmEntry;

pmEntry * phys_mem;
ptEntry ** page_dir;

void init_pd(){

	page_dir = (ptEntry **)malloc(SIZE*sizeof(ptEntry *)); //an array of ptEntry pointers
	int i;
	for(i=0; i<SIZE; i++){
		page_dir[i] = NULL; //initialize all entries to NULL
	}
}

ptEntry * init_pt(int size){

	ptEntry * pt = (ptEntry *)malloc(sizeof(ptEntry) * size);

	if(pt == NULL){
		return NULL;
	}
	int i;

	for(i = 0; i < size; i++){
		pt[i].invalid = 1;
		pt[i].frame = 0;
	}
	return pt;
}

void init_phys_mem(int size){
	phys_mem = (pmEntry *)malloc(size*sizeof(pmEntry));
	int i;
	for(i=0; i<size; i++){
		phys_mem[i].empty = 1; //initialize all frames of physical memory to empty
		phys_mem[i].clean = 1; //set frame as clean
		phys_mem[i].age = 0; //set age to 0
		phys_mem[i].value = 0; //set value to 0
	}
}
int findFrame(unsigned int address, char access_type){

	int i;

	for(i = 0; i < number_frames; i++){

		if(phys_mem[i].empty == 1){

			phys_mem[i].value = address;
			phys_mem[i].empty = 0;

			if(access_type == 'r'){
				phys_mem[i].clean = 1;
			}else{
				phys_mem[i].clean = 0;
			}
			return i;
		}
	}
	return -1;
}
int replaceFrame(char access_type, unsigned long int address){

	int i, victimFrame = 0;
	unsigned int youngest = UINT_MAX;

	for(i = 0; i < number_frames; i++){
		if((phys_mem[i].empty == 0)&&(phys_mem[i].age < youngest)&&(phys_mem[i].clean == 1)){ // find the page with the smallest age
		
			youngest = phys_mem[i].age;
			victimFrame = i;

		}
	}
	if(youngest==UINT_MAX){ //if no clean frame was found
		dirtyPageCount++; //a dirty page will be swapped out at this stage so we update it
		//loop trying to find a dirty frame
		for(i=0; i<number_frames; i++){
			if((phys_mem[i].empty == 0)&&(phys_mem[i].age < youngest)){ // find the page with the smallest age
				youngest = phys_mem[i].age; //update youngest
				victimFrame = i; //update victim frame
			}
		}
	}

	unsigned int pd = phys_mem[victimFrame].value >> 50;
	unsigned int pt = (phys_mem[victimFrame].value >> 33) & pageMask;
	page_dir[pd][pt].invalid = 1; //set victim page to invalid
	phys_mem[victimFrame].age = globalAge; //update victim page's age
	phys_mem[victimFrame].value = address;
	phys_mem[victimFrame].empty = 0;
	if(access_type == 'r'){
		phys_mem[victimFrame].clean = 1; //set frame to clean
	}else{
		phys_mem[victimFrame].clean = 0; //set frame to dirty
	}


	return victimFrame;

}
void manageMem(unsigned long logical_address, char access_type){

	//page table is 20 bits, page offset is 12 bits
	unsigned int pd = logical_address >> 50;				//22
	//printf("%i\n", sizeof(unsigned int));
	unsigned int pt = (logical_address >> 33) & pageMask;	//10
	unsigned int offset = logical_address & offsetMask;
	//offset is fine
	if(!((access_type == 'r')||(access_type == 'w'))){
		printf("Invalid access type\n");
		return;
	}
	unsigned long int upperLim = (unsigned long int)pow(2, 32);

	if((logical_address > upperLim) || (logical_address < 0)){
		printf("Invalid logical address\n");
		return;
	}
	if(page_dir[pd] == NULL){


		pageFault++;
		page_dir[pd] = init_pt(SIZE);



		int index = findFrame(logical_address, access_type);


		if(index != -1){
			page_dir[pd][pt].frame = index;			
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);			
		}else{
			int index = replaceFrame(access_type, logical_address);
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);			
		}
	}else if((page_dir[pd] != NULL)&&(page_dir[pd][pt].invalid == 1)){ //if entry in PT is invalid
		pageFault++; //increment page fault counter
		int index = findFrame(logical_address, access_type); //find an empty frame
		if(index!=-1){ //an empty frame was found
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
		}else{ //no empty frame was found
			int index = replaceFrame(access_type, logical_address); //page out
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
		}
	}else if((page_dir[pd] != NULL)&&(page_dir[pd][pt].invalid == 0)){ //if PD entry exists and PT entry is valid
		int index = page_dir[pd][pt].frame;
		if(access_type == 'w'){ //if the access type is write
			phys_mem[index].clean = 0; //set frame to dirty
		}
		phys_mem[index].age = globalAge;
		printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
	}
}

int main(int argc, char ** argv){

	if(argc != 2){
		printf("Incorrect command line arguments");
		return 0;
	}
	sscanf(&argv[1][0], "%d", &number_frames);

	init_phys_mem(number_frames);
	init_pd();

	char access_type;
	unsigned long logical_address;

	while(scanf("%c ", &access_type) != EOF){

		scanf("%lu ", &logical_address);
		globalAge++;

		manageMem(logical_address, access_type);
	}
}
