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
#include "ripTable.h"
struct listen{
	struct ripTable* mainTable;
};
struct triggered{
	struct ripTable* mainTable;
};
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
				struct sendData* info = prepareUpdateData(mainTable, currInt);
//				send((void*)info -> buffer, init -> mainTable -> mainNode -> nodeAddr, init -> mainTable -> mainNode -> nodePort, interface -> rnAddr, interface -> rnPort);
//				printf("sent data \n");
			}
			currInt = currInt -> next;
			sleep(5);
//			printf("woke uP!");
		}
	}
}

void* sendUpdate(void* data){
	 struct ripTable* init = (struct ripTable*) data;	
	 struct interface * currInt = init -> intList;
	 while(currInt != NULL){
                        if(currInt -> upDown == 0){
                                struct sendData* info = prepareUpdateData(init, currInt);
  //                              send((void*)info -> buffer,currInt -> vipSource, interface -> rnAddr, interface -> rnPort, interface -> vipDest, sendData -> flag, sendData -> size, 15);
                                printf(" sent data\n");
                        }
                        currInt = currInt -> next;
}
}
//void(struct ripTable*, char* destVIP){}
void* listeningThread(void* data){
	struct ripTable* table = (struct ripTable*)data;
	printf("I'm listening \n");	
}
//could be combined with  triggered updates
void* checkTimeout(void* data){
	struct ripTable* mainTable =  (struct ripTable*)data;	
	struct ripEntry* currEntry;
	while(1){
		currEntry = mainTable -> ripEntries;
			while(currEntry != NULL){
					if(currEntry -> updateTime > 11){
						currEntry -> cost = 17;
						currEntry -> updateTime = 0;
						printf("Route down!");
					}
					else{
						currEntry -> updateTime ++;
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
	while(currInt != NULL){
		if(addrToNumber(currInt -> vipSource) == addrToNumber(destVIP)){
			printf("Sent Data: %s \n", init -> buffer -> buffer);
			return;
		}
		currInt = currInt -> next;
	}	
	struct interface* route =  getRouteByDestVIP(destVIP,init ->  mainTable);
	//decrement TTL
//        send((void*)init -> buffer, init -> mainTable -> mainNode -> nodeAddr, interface -> rnPort, interface -> vipDest, sendData -> flag, sendData -> size, destVIP);
        printf("sent user data %s \n", init->buffer->buffer);
}

void* sendDataRequest(void* data){
	struct ripTable* mainTable = (struct ripTable*)data;
	struct interface* currInt = mainTable -> intList; 
	while(currInt != NULL){
                        if(currInt -> upDown == 0){
                                struct sendData* info = prepareUpdateData(mainTable, currInt);
                               // send((void*)"Request", currInt -> rnAddr, currInt -> rnPort, currInt -> vipDest, 2, 10, currInt -> vipDest);
                                printf("sent data request \n");
                        }

}
}
