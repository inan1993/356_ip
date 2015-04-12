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
int printVal = 0;
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
struct interface* getByInterfaceSrc(struct ripTable* mainTable, char* id){
	   struct interface* currInt = mainTable -> intList;
        while(currInt != NULL){
                if(addrToNumber(id) == addrToNumber(currInt -> vipSource)){
                        return currInt;
                }
                currInt = currInt -> next;
        }
	return NULL;

}
struct interface* getRouteByDestVIP(char* destVIP, struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
	struct nodeInfo* nextHop = (struct nodeInfo*) malloc(sizeof(nodeInfo));
	while(currEntry != NULL){
		if(!strcmp(destVIP, currEntry -> destVIP)){
//			nextHop -> nodeAddr = currEntry -> nextHop -> rnAddr;
//			nextHop -> nodePort = currEntry -> nextHop -> rnPort;
		//	printf("Next Hop: %s\n", currEntry -> destVIP);
			return currEntry -> nextHop;
		}
		currEntry = currEntry -> next;
	}
	printf("No path currently found to interface %s. Try again in a few seconds\n", destVIP);
	return NULL;
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
//	printf("Interface Not Found!!!!!!! %s\n", srcVIP);
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

int destInNode(struct ripTable* mainTable, char* dest){
	struct interface* currInt = mainTable -> intList;
//	printf("%s \n", dest);
	while(currInt ->next != NULL){
		currInt = currInt -> next;
	//	printf("Dest: %s  Node: %s \n", currInt -> vipSource, dest);
		if(addrToNumber(currInt -> vipSource) ==addrToNumber( dest) ){
	//		printf("in if\n");
			return 1;
		}
	}
	return 0;
}
void updateTable(struct ripUpdate* update, struct ripTable* mainTable){
	struct ripEntry* currEntry = mainTable -> ripEntries;
if(printVal)	printf("ID received: %s from %s with cost: %d at \n", addrToString(update -> destVIP), addrToString(update -> sourceVIP),update -> cost);
	int costOffset = 1;
	while(currEntry != NULL){
//		if(currEntry -> destVIP is on node && currEntry-> sourceVIP is on node){continue;}
	if(destInNode(mainTable, addrToString(update -> destVIP))){
		currEntry -> cost = 0;
		costOffset = 0;
//		currEntry = currEntry -> next;
//		continue;
	}
		if(!strcmp(addrToString(update -> destVIP), currEntry -> destVIP)){
			if(!strcmp(addrToString(update -> sourceVIP), currEntry -> sourceVIP)){
				if(update -> cost < 100)
					currEntry -> updateTime = 0;
				if(update-> cost < 16 ){
					currEntry -> cost = update -> cost * costOffset;
				}
				return;
			}
			else{
			if(update -> cost >= currEntry -> cost + currEntry -> nextHop -> upDown){
				return;
			}
			else{
				currEntry -> updateTime = 0;
				currEntry -> sourceVIP  = addrToString(update -> sourceVIP);
				currEntry -> cost = (update -> cost + currEntry -> nextHop -> upDown)*costOffset;
				currEntry -> nextHop = getInterfaceFromNextHopVIP(mainTable,addrToString( update -> sourceVIP));
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
	currEntry -> next = newEntry;
	newEntry -> prev = currEntry;
	newEntry -> cost = update -> cost;
	newEntry -> sourceVIP =addrToString(update -> sourceVIP);
	newEntry -> nextHop = getInterfaceFromNextHopVIP(mainTable, addrToString(update -> sourceVIP));
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
	int counter = 0;
	while(currEntry != NULL){
//		if(!strcmp(currEntry -> sourceVIP, receiverInt -> vipSource)){
//			printf("sources match %s \n",currEntry -> sourceVIP );
//		}
		if(!strcmp(currEntry -> sourceVIP, receiverInt -> vipDest) /* !strcmp(currEntry -> sourceVIP, receiverInt -> vipSource)*/){
			currUpdate[counter].cost = 17 + currEntry -> nextHop -> upDown;
		}
		else{
			currUpdate[counter].cost = currEntry -> cost +currEntry -> nextHop -> upDown +1;
		}
		currUpdate[counter].destVIP = addrToNumber(currEntry -> destVIP);
		currUpdate[counter].sourceVIP = addrToNumber(/*currEntry -> nextHop ->vipSource*/receiverInt -> vipSource);
//		if(strcmp(currEntry -> destVIP, currEntry -> sourceVIP)){
//			currUpdate[counter].cost = 1 + currEntry -> nextHop -> upDown;	
//		}
		currEntry = currEntry -> next;
		counter ++;
	}
	return buffer;
	
}
