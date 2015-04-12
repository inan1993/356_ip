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
#ifndef RIPTABLE_H
	#define RIPTABLE_H
	#include "ripTable.h"
#endif

//#include "ripTable.h"	
#ifndef SEND_H
	#define SEND_H
	#include"send.h"
#endif
#ifndef LISTEN_H
	#define LISTEN_H
	#include "listen.h"
#endif
//struct listen{
//	struct ripTable* mainTable;
//};
//struct triggered{
//	struct ripTable* mainTable;
//};
struct user{
	struct ripTable* mainTable;
	int ttl;
	struct sendData* buffer;
	char * destVIP;
};
void* triggeredUpdates(void* data){
	struct ripTable* mainTable = (struct ripTable*)data;
	while(1){
	struct interface* currInt = mainTable -> intList;
		while(currInt != NULL){
			if(currInt -> upDown == 0){
				struct sendData* info = prepareUpdateData(mainTable, currInt, 1);
		//	printf("conn Port: %d, srcVIP: %s, destVIP: %s \n", currInt ->rnPort, currInt -> vipSource, currInt -> vipDest);
				sender((void*)(info -> buffer),currInt -> vipSource, currInt -> rnAddr, currInt -> rnPort, currInt -> vipDest, 1, info -> size, 15);

//				printf("sent data \n");
			}
			currInt = currInt -> next;
//			printf("woke uP!");
		}
		;sleep(5);
	}
}

void* sendUpdate(void* data){
	 struct ripTable* init = (struct ripTable*) data;	
	 struct interface * currInt = init -> intList;
	 while(currInt != NULL){
                        if(currInt -> upDown == 0){
                                struct sendData* info = prepareUpdateData(init, currInt, 0);
                                sender((void*)(info -> buffer),currInt -> vipSource, currInt -> rnAddr, currInt -> rnPort, currInt -> vipDest, info -> flag, info -> size, 15);
  //                              printf(" sent update!\n");
                        }
                        currInt = currInt -> next;
}
}
//void(struct ripTable*, char* destVIP){}
void* listeningThread(void* data){
	struct ripTable* table = (struct ripTable*)data;
	listening(table -> mainNode -> nodeAddr, table -> mainNode -> nodePort, table);
//	printf("I'm listening \n");	
}
//could be combined with  triggered updates
void* checkTimeout(void* data){
	struct ripTable* mainTable =  (struct ripTable*)data;	
	struct ripEntry* currEntry;
	while(1){
		currEntry = mainTable -> ripEntries;
			while(currEntry != NULL){
					if(currEntry -> updateTime > 11){
						if(!destInNode(mainTable, currEntry -> destVIP))
						currEntry -> cost = 42;
						//currEntry -> updateTime = 0;
						currEntry -> updateTime ++;
						currEntry -> sourceVIP = "NULL IP";
					}
					else{
						currEntry -> updateTime ++;
					}
					if(currEntry -> updateTime == 12){
//						printf("route down! \n");
					}
				currEntry = currEntry -> next;
			}
//	printf("incrementing all timers! \n");
	sleep(1);
	}
}

void* sendUserData(void* data){
	struct user* init = (struct user*)data;
	char* destVIP = init -> destVIP;
	struct interface* currInt = init -> mainTable -> intList;
	struct sendData* info = init -> buffer;
	int TTL = init -> ttl;
	while(currInt != NULL){
		if(addrToNumber(currInt -> vipSource) == addrToNumber(destVIP)){
			printf("%s \n", init -> buffer -> buffer);
			return data;
		}
		currInt = currInt -> next;
	}	
	struct interface* route =  getRouteByDestVIP(destVIP,init ->  mainTable);
	if(!route == NULL){
	 sender((void*)(info -> buffer),route->vipSource, route->rnAddr, route->rnPort, destVIP, info -> flag, info -> size, TTL);
}
}

void* sendDataRequest(void* data){
	struct ripTable* mainTable = (struct ripTable*)data;
	struct interface* currInt = mainTable -> intList; 
	while(currInt != NULL){
                        if(currInt -> upDown == 0){
                                struct sendData* info = prepareUpdateData(mainTable, currInt, 0);
//				printf("sent data request! \n");
                        	 sender((void*)(info -> buffer),currInt -> vipSource, currInt -> rnAddr, currInt -> rnPort, currInt -> vipDest, 2, 40, 15);
 
//			       send((void*)"Request", currInt -> rnAddr, currInt -> rnPort, currInt -> vipDest, 2, 10, currInt -> vipDest);
                                currInt = currInt -> next;
//				printf("sent data request \n");
                        }

}
}
