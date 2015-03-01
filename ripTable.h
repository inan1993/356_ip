#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
struct ripEntry{
        char* destVIP;
        struct interface* nextHop;
        int cost;
        time_t updateTime;
        struct ripEntry* next;
        struct ripEntry* prev;
}ripEntry;

struct ripTable{
	struct ripEntry* ripEntries;
    struct nodeInfo* mainNode;
	struct interface* intList;
}ripTable;

struct ripUpdate{
	unsigned long destVIP;
	int cost;
	unsigned long sourceVIP;
};


struct interface* getRouteByDestVIP(char* destVIP, struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	struct nodeInfo* nextHop = (struct nodeInfo*) malloc(sizeof(nodeInfo));
	while(currEntry != NULL){
		if(!strcmp(destVIP, currEntry -> destVIP)){
//			nextHop -> nodeAddr = currEntry -> nextHop -> rnAddr;
//			nextHop -> nodePort = currEntry -> nextHop -> rnPort;
			return currEntry -> nextHop;
		}
		currEntry = currEntry -> next;
	}
}
void initializeTable(struct nodeInfo* mainNode, struct interface* intList, struct ripTable* mainTable){
	struct interface* currInt = intList;
	struct ripEntry* currEntry = (struct ripEntry*)malloc(sizeof(ripEntry));
	currEntry -> prev = NULL;
	struct ripEntry* firstEntry = currEntry;
	while(currInt != NULL){
		currEntry -> destVIP = currInt -> vipDest;
	        currEntry -> nextHop = currInt;
        	currEntry -> cost = 1;
		if(currInt -> next != NULL){
			struct ripEntry* nextEntry = (struct ripEntry*)malloc(sizeof(ripEntry));
			currEntry -> next = nextEntry;
			nextEntry -> prev = currEntry;
			currEntry = nextEntry;
		}
		currInt = currInt -> next;
	}
	mainTable -> mainNode = mainNode;
	mainTable -> ripEntries = firstEntry;
	mainTable -> intList = intList;
}

struct interface* getInterfaceFromNextHopVIP(struct ripTable* mainTable,char* srcVIP){
	struct interface* currList = mainTable -> intList;
	while(currList != NULL){
		if(strcmp(currList -> vipDest,srcVIP)){
			return currList;
		}
		currList = currList -> next;
	}
}
void updateTable(struct ripUpdate* update, struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	while(currEntry != NULL){
		if(!strcmp(update -> destVIP, currEntry -> destVIP)){
			if(update -> cost == -1){
				if(currEntry -> next != NULL && currEntry ->prev != NULL){
					currEntry->prev->next = currEntry -> next;
					currEntry -> next -> prev = currEntry -> prev;
				}
				else if(currEntry -> prev == NULL){
					currEntry -> next -> prev = NULL;
					mainTable -> ripEntries = currEntry -> next;
				}
				else if(currEntry -> next == NULL){
					currEntry -> prev -> next = NULL;
					currEntry = currEntry->prev;
				}
				printf("REMOVING SOMETHING");
			}
			if(update -> cost >= currEntry -> cost){
				printf("returned and did nothing, currEntry -> nextHop \n");
				return;
			}
			else{
				currEntry -> cost = update -> cost;
				currEntry -> nextHop = getInterfaceFromNextHopVIP(mainTable, update -> sourceVIP);
				printf("returned and updated %p \n", currEntry -> nextHop);
				return;
			}
		}
		if(currEntry -> next == NULL){
			break;
		}
		
		currEntry = currEntry -> next;
	} 
	struct ripEntry* newEntry = (struct ripEntry*) malloc(sizeof(ripTable));
	currEntry -> next = newEntry;
	newEntry -> prev = currEntry;
	newEntry -> cost = update -> cost;
	newEntry -> destVIP = update -> destVIP;
	printf("no new matches \n");
	newEntry -> nextHop = getInterfaceFromNextHopVIP(mainTable, update -> sourceVIP);
	printf("nextHop: %p", newEntry->nextHop);
}
int getTableLength(struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	int i = 0;
	while(currEntry != NULL){
		i++;
		currEntry = currEntry -> next;
	}
	return i;
}
//Integer for the number of updates

struct ripUpdate* prepareUpdateData(struct ripTable* mainTable, struct interface* receiverInt){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	struct ripUpdate* currUpdate = (struct ripUpdate*)malloc(sizeof(struct ripUpdate)*getTableLength(mainTable));
	int counter = 0;
	while(currEntry != NULL){
		currUpdate[counter].destVIP = currEntry ->destVIP;
		printf("where it breaks: %p  %d\n", currEntry -> nextHop,counter);
		if(!(currEntry -> nextHop -> upDown)){
			currUpdate[counter].cost = -1;//currEntry -> cost + 1;
		}
		else if(!strcmp(currEntry -> nextHop -> vipDest, receiverInt ->vipDest)){
			currUpdate[counter].cost = 16;
		}
		else{
			currUpdate[counter].cost = currEntry -> cost + 1;
		}
		currUpdate[counter].sourceVIP = currEntry -> nextHop ->vipSource;	
		currEntry = currEntry -> next;
		printf("destIP: %s, cost: %d, sourceIP: %s \n", currUpdate[counter].destVIP, currUpdate[counter].cost, currUpdate[counter].sourceVIP);
		counter ++;
	}
	printf("\n");
	return currUpdate;
	
}
