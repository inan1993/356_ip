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

char* addrToString(unsigned long updateAddr){
	char* tempy = malloc(sizeof(char) * 100);
	struct in_addr* addr1 =(struct in_addr*) malloc(sizeof(struct in_addr));
        addr1->s_addr = updateAddr;
	strcpy(tempy, inet_ntoa(*addr1));
	printf("tempy: %s original: %lu\n", tempy,updateAddr );
	return tempy;

}
unsigned long addrToNumber(char* ip){

}
struct ripUpdate* updateFromInterface(struct interface* inter, int cost){
	struct ripUpdate* update = (struct ripUpdate*) malloc(sizeof(struct ripUpdate));
	update -> destVIP = inet_addr(inter -> vipDest);
	update -> cost = cost;
	update -> sourceVIP = inet_addr(inter -> vipSource);
}
struct interface* getInterfaceByID(int id, struct ripTable* mainTable){
	struct interface* currInt = mainTable -> intList;
	while(currInt != NULL){
		if(currInt -> interId == id){
			return currInt;
		}
		currInt = currInt -> next;
	}
}
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
//unused
void updateLocally(struct interface* changedInterface, int cost, struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	while(currEntry != NULL){
		if(changedInterface == currEntry -> nextHop){
			currEntry -> cost = cost;
		}
		currEntry = currEntry -> next;
	}
}
void updateTable(struct ripUpdate* update, struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	while(currEntry != NULL){
		if(!strcmp(addrToString(update -> destVIP), currEntry -> nextHop -> vipSource)){
			if(!strcmp(addrToString(update -> sourceVIP), currEntry -> nextHop -> vipDest)){
				currEntry -> cost = update -> cost;
			}
			else{
			if(update -> cost >= currEntry -> cost + currEntry -> nextHop -> upDown){
				printf("returned and did nothing, currEntry -> nextHop \n");
				return;
			}
			else{
				currEntry -> cost = update -> cost;
				currEntry -> nextHop = getInterfaceFromNextHopVIP(mainTable,addrToString( update -> sourceVIP));
				printf("returned and updated %p \n", currEntry -> nextHop);
				return;
			}
		}
		}
		if(currEntry -> next == NULL){
			if(update -> cost == -1){ return;}
			break;
		}
		
		currEntry = currEntry -> next;
	} 
	struct ripEntry* newEntry = (struct ripEntry*) malloc(sizeof(ripTable));
	currEntry -> next = newEntry;
	newEntry -> prev = currEntry;
	newEntry -> cost = update -> cost;
	newEntry -> destVIP = addrToString(update -> destVIP);
	printf("no new matches destVIP: %s \n",addrToString(update ->  destVIP));
	newEntry -> next = NULL;
	newEntry -> nextHop = getInterfaceFromNextHopVIP(mainTable, addrToString(update -> sourceVIP));
	printf("nextHop: %p %p", newEntry->nextHop, newEntry -> next);
}
int getTableLength(struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	int i = 0;
	while(currEntry != NULL){
		i++;
//		printf("i: %d\n" ,i);
		currEntry = currEntry -> next;
	}
	return i;
}
//Integer for the number of updates

struct ripUpdate* prepareUpdateData(struct ripTable* mainTable, struct interface* receiverInt){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	struct ripUpdate* currUpdate = (struct ripUpdate*)malloc(sizeof(struct ripUpdate)*getTableLength(mainTable));
	int counter = 0;
	printf("\n%d\n", getTableLength(mainTable));
	while(currEntry != NULL){
//		char* temp = malloc(sizeof(char) * 100);
//		strcpy(temp, currEntry -> destVIP);
		currUpdate[counter].destVIP = inet_addr(currEntry -> destVIP);
		currUpdate[counter].cost = currEntry -> cost +currEntry -> nextHop -> upDown +1;
		currUpdate[counter].sourceVIP = inet_addr(currEntry -> nextHop ->vipSource);
		printf("dest:%s ", currEntry -> destVIP);	
		currEntry = currEntry -> next;
		printf("destIP: %s, cost: %d, sourceIP: %s \n",addrToString(currUpdate[counter].destVIP), currUpdate[counter].cost, addrToString(currUpdate[counter].sourceVIP));
		counter ++;
	}
	printf("\n");
	return currUpdate;
	
}
