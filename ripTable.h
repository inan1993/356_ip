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
#include "parser.h"

struct ripEntry{
        char* destVIP;
        char* sourceVIP;
	struct interface* nextHop;
        int cost;
        int updateTime;
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
struct sendData{
	int size;
	int flag;
	char buffer[];
};

char* addrToString(unsigned long updateAddr){
	char* tempy = malloc(sizeof(char) * 100);
	struct in_addr* addr1 =(struct in_addr*) malloc(sizeof(struct in_addr));
        addr1->s_addr = updateAddr;
	strcpy(tempy, inet_ntoa(*addr1));
//	printf("tempy: %s original: %lu\n", tempy,updateAddr );
	return tempy;

}
unsigned long addrToNumber(char* ip){
	struct in_addr* temper =(struct in_addr*)malloc(sizeof(struct in_addr));
	inet_aton(ip,temper);
	return temper -> s_addr;
}
struct ripUpdate* updateFromInterface(struct interface* inter, int cost){
	struct ripUpdate* update = (struct ripUpdate*) malloc(sizeof(struct ripUpdate));
	update -> destVIP = addrToNumber(inter -> vipDest);
	update -> cost = cost;
	update -> sourceVIP = addrToNumber(inter -> vipSource);
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
int containsInterfaceSrc(struct ripTable* mainTable, char* id){
	   struct interface* currInt = mainTable -> intList;
        while(currInt != NULL){
                if(addrToNumber(id) == addrToNumber(currInt -> vipSource)){
                        return 1;
                }
                currInt = currInt -> next;
        }
	return 0;

}
struct interface* getRouteByDestVIP(char* destVIP, struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	struct nodeInfo* nextHop = (struct nodeInfo*) malloc(sizeof(nodeInfo));
	while(currEntry != NULL){
		if(!strcmp(destVIP, currEntry -> destVIP)){
//			nextHop -> nodeAddr = currEntry -> nextHop -> rnAddr;
//			nextHop -> nodePort = currEntry -> nextHop -> rnPort;
			printf("Next Hop\n");
			return currEntry -> nextHop;
		}
		currEntry = currEntry -> next;
	}
	printf("Not Found Next Hop\n");
}
void initializeTable(struct nodeInfo* mainNode, struct interface* intList, struct ripTable* mainTable){
	struct interface* currInt = intList;
	struct ripEntry* currEntry = (struct ripEntry*)malloc(sizeof(ripEntry));
	currEntry -> prev = NULL;
	struct ripEntry* firstEntry = currEntry;
	while(currInt != NULL){
		currEntry -> destVIP = currInt -> vipSource;
	        currEntry -> nextHop = currInt;
		currEntry -> sourceVIP = "NULL_IP";
        	currEntry -> cost = 0;
		if(currInt -> next != NULL){
			struct ripEntry* nextEntry = (struct ripEntry*)malloc(sizeof(ripEntry));
			currEntry -> next = nextEntry;
			nextEntry -> prev = currEntry;
			currEntry = nextEntry;
		}
//		printf("currInt next %p \n", currInt -> next);
		currInt = currInt -> next;
	}
	mainTable -> mainNode = mainNode;
	mainTable -> ripEntries = firstEntry;
	mainTable -> intList = intList;
}

struct interface* getInterfaceFromNextHopVIP(struct ripTable* mainTable,char* srcVIP){
	struct interface* currList = mainTable -> intList;
	while(currList != NULL){
//		printf("VIP: %s %s \n", currList -> vipDest, srcVIP);
		if(!strcmp(currList -> vipDest,srcVIP)){
			return currList;
		}
		currList = currList -> next;
	}
	printf("Interface Not Found!!!!!!! %s\n", srcVIP);
	currList -> next = currList;
}

//unused
// void updateLocally(struct interface* changedInterface, int cost, struct ripTable* mainTable){
// 	struct ripEntry* currEntry = mainTable -> ripEntries;
// 	while(currEntry != NULL){
// 		if(changedInterface == currEntry -> nextHop){
// 			currEntry -> cost = cost;
// 		}
// 		currEntry = currEntry -> next;
// 	}
// }

void updateTable(struct ripUpdate* update, struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	printf("ID received: %s from %s with cost: %d at \n", addrToString(update -> destVIP), addrToString(update -> sourceVIP),update -> cost);
	while(currEntry != NULL){
//		printf("updateDest; %s, entryDest: %s, updatesrc: %s, entrySrc: %s \n", addrToString(update -> destVIP),currEntry -> nextHop -> vipDest, addrToString(update -> sourceVIP), currEntry -> destVIP);
//		if(update -> destVIP != addrToNumber())
//		if(!containsInterfaceSrc(mainTable, currEntry -> destVIP)){return;}
		if(!strcmp(addrToString(update -> destVIP), currEntry -> destVIP)){
	//		printf("reset time for %s \n",addrToString( update -> destVIP) );
			currEntry -> updateTime = 0;
			if(!strcmp(addrToString(update -> sourceVIP), currEntry -> sourceVIP)){
				if(update-> cost < 16 ||update -> cost >= 100){
					currEntry -> cost = update -> cost;
				}
				return;
			}
			else{
			if(update -> cost >= currEntry -> cost + currEntry -> nextHop -> upDown){
//				printf("returned and did nothing, currEntry -> nextHop \n");
				return;
			}
			else{
				currEntry -> cost = update -> cost;
				currEntry -> nextHop = getInterfaceFromNextHopVIP(mainTable,addrToString( update -> sourceVIP));
//				printf("returned and updated %p \n", currEntry -> nextHop);
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
	struct ripEntry* newEntry = (struct ripEntry*) malloc(sizeof(ripEntry));
//	printf("currEntry: %p %d \n", currEntry, 1);
	currEntry -> next = newEntry;
	newEntry -> prev = currEntry;
	newEntry -> cost = update -> cost;
	newEntry -> sourceVIP =addrToString(update -> sourceVIP);
	newEntry -> nextHop = getInterfaceFromNextHopVIP(mainTable, addrToString(update -> sourceVIP));
//	printf("nextHop: %p %p", newEntry->nextHop, newEntry -> next);
//	printf("no new matches destVIP: %s \n",newEntry -> destVIP);
	newEntry -> destVIP = addrToString(update -> destVIP);
	newEntry -> next = NULL;
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

void* prepareUpdateData(struct ripTable* mainTable, struct interface* receiverInt,int reversePoisonFlag){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	struct sendData* buffer = malloc(sizeof(int)* 2 + getTableLength(mainTable) * sizeof(struct ripUpdate));
	buffer -> flag = 1;
	buffer -> size = getTableLength(mainTable); 
	struct ripUpdate* currUpdate = (struct ripUpdate*)(buffer -> buffer);
	//(struct ripUpdate*)malloc(sizeof(struct ripUpdate)*getTableLength(mainTable));
	int counter = 0;
//	printf("\nTable length:%d\n", getTableLength(mainTable));

	
	while(currEntry != NULL){
//		char* temp = malloc(sizeof(char) * 100);
//		strcpy(temp, currEntry -> destVIP);
//		if(currEntry -> nextHop == receiverInt && strcmp(currEntry->destVIP, receiverInt -> vipDest) && reversePoisonFlag){
//			printf("Reverse Poison Update\n");
		if(!strcmp(currEntry -> sourceVIP, receiverInt -> vipDest)){
			currUpdate[counter].cost = 17 + currEntry -> nextHop -> upDown;
//			printf("reverse poison: \n" );
		}
		else{
			currUpdate[counter].cost = currEntry -> cost +currEntry -> nextHop -> upDown +1;
//			if(!strcmp(currEntry->destVIP, receiverInt -> vipDest)){
//				 currUpdate[counter].cost = 1 + currEntry -> nextHop -> upDown;
//			}
		}
		currUpdate[counter].destVIP = addrToNumber(currEntry -> destVIP);
//		currUpdate[counter].cost = currEntry -> cost +currEntry -> nextHop -> upDown +1;
		currUpdate[counter].sourceVIP = addrToNumber(/*currEntry -> nextHop ->vipSource*/receiverInt -> vipSource);
//		printf("dest:%s\n ", currEntry -> destVIP);	
		currEntry = currEntry -> next;
//		printf("destIP: %s, cost: %d, sourceIP: %s \n",addrToString(currUpdate[counter].destVIP), currUpdate[counter].cost, addrToString(currUpdate[counter].sourceVIP));
		counter ++;
	}
//	printf("\n-----End of Creating Update Array-----\n");

	return buffer;
	
}
