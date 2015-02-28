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
}ripTable;

struct nodeInfo* getRouteByDestVIP(char* destVIP){}
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
}
void updateTable(struct ripEntry* receivedUpdate){}
