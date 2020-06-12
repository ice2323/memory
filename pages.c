 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define SIZE (int)pow(2,12) 

typedef struct pageTableEntry{
	int invalid; 
	int frame; 
}ptEntry;

typedef struct physicalMemoryEntry{
	int empty; 
	int clean; 
	int age; 
	unsigned long int value; 
}pmEntry;

int number_frames; 
ptEntry ** page_dir;
pmEntry * phys_mem; 
int globalAge = 0; 

int pageMask = 0x3FF; 
int offsetMask = 0xFFF; 
int pageFault = 0;
int dirtyPageCount = 0; 

void init_pd(){
	page_dir = (ptEntry **)malloc(SIZE*sizeof(ptEntry *)); 
	
	for(int i = 0; i < SIZE; i++){
		page_dir[i] = NULL; 
	}
}


ptEntry * init_pt(int size){
	ptEntry * pt = (ptEntry *)malloc(sizeof(ptEntry) * size);
	if(pt == NULL){
		return NULL;
	}
	
	for(int i = 0; i<size; i++){
		pt[i].invalid = 1; 
		pt[i].frame = 0; 
	}
	return pt;
}

void createMemory(int size){

	phys_mem = (pmEntry *)malloc(size*sizeof(pmEntry));

	for(int i=0; i<size; i++){
		phys_mem[i].empty = 1; 
		phys_mem[i].clean = 1; 
		phys_mem[i].age = 0;
		phys_mem[i].value = 0; 
	}
}

int findFrame(unsigned int address, char access_type){
	
	for(int i=0; i<number_frames; i++){
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
	int victimFrame = 0;
	unsigned int youngest = UINT_MAX;

	for(int i=0; i<number_frames; i++){
		if((phys_mem[i].empty == 0)&&(phys_mem[i].age < youngest)&&(phys_mem[i].clean == 1)){ 
			youngest = phys_mem[i].age;
			victimFrame = i; 
			}
	}

	if(youngest==UINT_MAX){ 
		dirtyPageCount++; 
		
		for(int i=0; i<number_frames; i++){
			if((phys_mem[i].empty == 0)&&(phys_mem[i].age < youngest)){ 
				youngest = phys_mem[i].age; 
				victimFrame = i; 
			}
		}
	}

	unsigned int pd = phys_mem[victimFrame].value >> 20;
	unsigned int pt = (phys_mem[victimFrame].value >> 12) & pageMask;
	page_dir[pd][pt].invalid = 1; 
	phys_mem[victimFrame].age = globalAge; 
	phys_mem[victimFrame].value = address;
	phys_mem[victimFrame].empty = 0;
	if(access_type == 'r'){
		phys_mem[victimFrame].clean = 1; 
	}else{
		phys_mem[victimFrame].clean = 0; 
	}


	return victimFrame;
}

void manageMem(unsigned long logical_address, char access_type){


	unsigned int pd = logical_address >> 20;
	unsigned int pt = (logical_address >> 12) & pageMask;
	unsigned int offset = logical_address & offsetMask;




	if(!((access_type == 'r')||(access_type == 'w'))){
		printf("Invalid access type\n");
		return;
	}
	unsigned long int upperLim = (unsigned long int)pow(2,32);
	if((logical_address > upperLim) || (logical_address < 0)){
		printf("Invalid logical address\n");
		return;
	}
	if(page_dir[pd] == NULL){ 
		pageFault++;
		page_dir[pd] = init_pt(SIZE); 
		int index = findFrame(logical_address, access_type);
		if(index!=-1){ 
			page_dir[pd][pt].frame = index; 
			page_dir[pd][pt].invalid = 0; 
			phys_mem[index].age = globalAge;
			printf("%s %lu %s %u\n", "Logical address", logical_address, "-> physical address", index + offset);

			//printf("Logical address %lu -> physical address %u\n", logical_address, (index<<12)+offset);
		}else{ 
			int index = replaceFrame(access_type, logical_address); 
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("%s %lu %s %u\n", "Logical address", logical_address, "-> physical address", index + offset);
		}

	}else if((page_dir[pd] != NULL)&&(page_dir[pd][pt].invalid == 1)){ 
		pageFault++; 
		int index = findFrame(logical_address, access_type); 
		if(index!=-1){ 
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("%s %lu %s %u\n", "Logical address", logical_address, "-> physical address", index + offset);
		}else{ 
			int index = replaceFrame(access_type, logical_address); 
			page_dir[pd][pt].frame = index;
			page_dir[pd][pt].invalid = 0;
			phys_mem[index].age = globalAge;
			printf("%s %lu %s %u\n", "Logical address", logical_address, "-> physical address", index + offset);
		}
	}else if((page_dir[pd] != NULL) && (page_dir[pd][pt].invalid == 0)){ 
		int index = page_dir[pd][pt].frame;
		if(access_type == 'w'){ 
			phys_mem[index].clean = 0; 
		}
		phys_mem[index].age = globalAge;
		printf("%s %lu %s %u\n", "Logical address", logical_address, "-> physical address", index + offset);
	}
}

int main(int argc, char ** argv){

	if(argc != 2){
		printf("%s\n", "Please provide number of frames as a command line arg!");
		return 0;
	}

	number_frames = atoi(argv[1]);

	createMemory(number_frames);
	init_pd();

	//variables to hold access type and the logical address
	char access_type;
	unsigned long logical_address;

	//start to read in user input
	while(scanf("%c ", &access_type) != EOF){
		scanf("%lu ", &logical_address);
		globalAge++;
		manageMem(logical_address, access_type);
	}

	//print out the total number of page faults
	printf("\n%s %d\n\n", "Number of page faults:", pageFault);

	return 0;
}
