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
#ifndef THREADMETHODS_H
#define THREADMETHODS_H
#include "threadMethods.h"
#endif

#ifndef THREADMETHODS_H
#define THREADMETHODS_H
//#include "send.h"
#endif
#ifndef LIS
#define LIS
//#include "listen.h"
#endif
//#include "listen.h"
//uint16_t command;
//uint16_t num_entries;
//struct {
//uint32_t cost;
//uint32_t address;
//} entries[num_entries];
struct threadInit{
	struct nodeInfo mainNode;
	struct ripTable* mainTable;	
};

void testUpdate(char* dest, int cost, char* source, struct ripTable* mainTable){
        struct ripUpdate* testUpdate = (struct ripUpdate*)malloc(sizeof(struct ripUpdate));

        testUpdate -> destVIP =addrToNumber(dest);
        testUpdate -> cost = cost;
        testUpdate -> sourceVIP = addrToNumber(source);
        updateTable(testUpdate,mainTable);
//      printTable(mainTable);
}

void printTable(struct ripTable* table){
	printf("\n");
	struct ripEntry* entries = table -> ripEntries;
	struct nodeInfo* mainNode = table -> mainNode;
//	printf("Node Addr: %s Node Port: %d \n", mainNode -> nodeAddr, mainNode -> nodePort);
	struct ripEntry* currEntry = table ->ripEntries;
	while(currEntry != NULL){
	if(currEntry -> cost <= 16){
		if(currEntry -> cost >=1)		
		printf("%s\t%d\t%d\n",currEntry -> destVIP, currEntry -> nextHop -> interId, currEntry -> cost);
		currEntry = currEntry -> next;
	}
	else if (currEntry -> cost == 17){
		printf("%s\t%d\t%d\n",currEntry -> destVIP, currEntry -> nextHop -> interId, 16);
	}
	}
//	printf("size read: %d \n", getTableLength(table));  
	printf("\n");
}
//void* reader(void* data){
//	struct returnInfo* allInfo = (struct returnInfo*) data;
//	struct nodeInfo* mainNode = allInfo -> mainNode;
//	struct interface* intList = allInfo -> interfaceList;
//	printf("Read thread run \n");
//	return NULL;
//}
//void* listener(void* data){
//	struct returnInfo* allInfo = (struct returnInfo*) data;
  //      struct nodeInfo* mainNode= allInfo -> mainNode;
  //      struct interface* intList = allInfo -> interfaceList;
//	printf("Listener thread run \n");
//	return NULL;
//}
int getThirdArg(char* string, char delimiter, char* thirdArg){
	int i = 0;
	int count = 0;
	int index = 0;
	int length = 0;
	while(string[i] != '\0'){
		if(string [i] == delimiter && count < 2){
			count ++;
			index = i;
		}
		if(count >= 2){
			
			length ++;
		}
		i++;
	}
	if(length < 16){
	strcat(thirdArg, "                ");
	}
	length = length + 16;
	strcpy(thirdArg, &string[index]);
//	printf("thirdArg:  %s:%p: \n",thirdArg, thirdArg);
	return length;
}
int main(int argc, char ** argv){
	struct returnInfo* returnData = parser(argc, argv);
	struct ripTable* mainTable =(struct ripTable*) malloc(sizeof(ripTable));
	initializeTable(returnData -> mainNode, returnData -> interfaceList, mainTable);
//	printTable(mainTable);
//	testUpdate("10.10.10.10",1,"0.0.0.168" , mainTable);
//	prepareUpdateData(mainTable, mainTable -> intList);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	pthread_t* updateThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_t* periodicThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_t* listenThread = (pthread_t*) malloc(sizeof(pthread_t));
	pthread_t* timeoutThread = (pthread_t*) malloc(sizeof(pthread_t));
	int rc = 0;
	//Start listening/dealing with updating
	if(rc = pthread_create(periodicThread, &attr, triggeredUpdates, (void*)mainTable)){
		printf("thread creation error %d\n", rc);	
	}
	if(rc = pthread_create(updateThread, &attr, sendDataRequest,(void*)mainTable)){
            printf("thread creation error%d\n", rc);
    }
	//triggered updates
	 if(rc = pthread_create(listenThread, &attr, listeningThread, (void*)mainTable)){
               printf("thread creation error%d\n", rc);
	    }
	 if(rc = pthread_create(timeoutThread, &attr, checkTimeout,(void*)mainTable)){
             printf("thread creation error%d\n", rc);
       }


char buffer[256];
while(1){
	char * origBuffer = malloc(sizeof(char) * 256);
	fgets( buffer, 256, stdin);
	strcpy(origBuffer, buffer);
	char* command = strtok(buffer, " ");
	char* firstArgument = strtok(NULL, " "); 
	if(!strcmp(origBuffer, "update\n")){
		 pthread_t* tempThread = (pthread_t*) malloc(sizeof(pthread_t));
	        int rc = 0;
      		if(rc = pthread_create(tempThread, &attr, sendUpdate, (void*)mainTable)){
              	printf("thread creation error %d\n", rc);       
      }
		
	}
	if(!strcmp(origBuffer, "print\n")){
		printVal = !printVal;
	}
	if(!strcmp(origBuffer, "ifconfig\n")){
		struct interface* currInt = returnData -> interfaceList;
		while(currInt != NULL){
			printf("%d \t %s \t", currInt -> interId, currInt -> vipSource);
			if(currInt -> upDown == 0) printf("up");
			else printf("down");
			currInt = currInt -> next;
			printf("\n");
		}		
	}
	if(!strcmp(origBuffer, "routes\n")){printTable(mainTable);}
	 if(!strcmp(command,"up")){
		int i = 0;
		int dontDoThis = 0;
		int arg = atoi(firstArgument);
		struct interface* currInt = returnData -> interfaceList;
		for(i = 0; i <arg-1; i ++){
			if(currInt -> next == NULL){ printf("Interface %d not found!\n", arg);
			dontDoThis = 1;
			break;
			}
			currInt = currInt -> next;
		}
		if(dontDoThis) continue;
		currInt -> upDown = 0;
		 pthread_t* upThread = (pthread_t*) malloc(sizeof(pthread_t));
                         int rc = 0;
                if(rc = pthread_create(upThread, &attr, sendUpdate, (void*)mainTable)){
                	printf("thread creation error %d\n", rc);
        	}

		printf("Interface %d up!\n", arg);
	}
	if(!strcmp(command,"down")){
		int i = 0;
		int dontDoThis = 0;
                int arg = atoi(firstArgument);
                struct interface* currInt = returnData -> interfaceList;
		for(i = 0; i <arg-1; i ++){
		if(currInt -> next == NULL){ printf("Interface %d not found!\n", arg);
                        dontDoThis = 1;
			break;
			}
			currInt = currInt -> next;
                }
		if(dontDoThis) continue;
                currInt -> upDown = 100;
		pthread_t* downThread = (pthread_t*) malloc(sizeof(pthread_t));
       			 int rc = 0;
       		if(rc = pthread_create(downThread, &attr, sendUpdate, (void*)mainTable)){
                printf("thread creation error %d\n", rc);
        }

		
		printf("Interface %d down!\n", arg);
	}
	if(!strcmp(command, "send")){
//		printf("origBuffer %s \n", origBuffer);
		char* ipString = firstArgument;
		struct sendData* payload = (struct sendData*)(malloc(sizeof(char)*256 + sizeof(int)*2));	
	//	printf("payolad buffer:%p \n ", payload -> buffer);
		int size = getThirdArg(origBuffer, ' ', payload -> buffer);
//		printf("size: %s \n", payload -> buffer);	
	//void* buffer = malloc(sizeof(char) * size+sizeof(int));
		payload -> size = size;
		payload -> flag = 3;
		struct interface* nextHop = getRouteByDestVIP(ipString,mainTable);
	//	printf("made it past that...");
		if(nextHop == NULL) continue;
		struct nodeInfo* nextHopInfo = (struct nodeInfo*)malloc(sizeof(struct nodeInfo));
		nextHopInfo -> nodeAddr = nextHop -> rnAddr;
		nextHopInfo -> nodePort = nextHop -> rnPort;
	//	printf("NEXT HOP: %s:%d", nextHopInfo -> nodeAddr,nextHopInfo -> nodePort);
		pthread_t* userThread = (pthread_t *)malloc(sizeof(pthread_t));
		struct user* user = (struct user*)malloc(sizeof(struct user));
		user -> mainTable = mainTable;
		user ->	ttl = 15;
		user -> destVIP = ipString;
		user -> buffer = payload;	
		//launch thread to send data -- need to include route data
		if(rc = pthread_create(userThread, &attr, sendUserData,(void*)user)){
       	         printf("thread creation error%d\n", rc);

        }
//	printf("-->\n");
	}
}
}
