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
void testUpdate(struct ripTable* mainTable){
        struct ripUpdate* testUpdate = (struct ripUpdate*)malloc(sizeof(struct ripUpdate));
        testUpdate -> destVIP = ".89.230";
        testUpdate -> cost = 1;
        testUpdate -> sourceVIP = "10.116.89.157";
        updateTable(testUpdate,mainTable);
//      printTable(mainTable);
}

void printTable(struct ripTable* table){
	printf("\n");
	struct ripEntry* entries = table -> ripEntries;
	struct nodeInfo* mainNode = table -> mainNode;
	printf("Node Addr: %s Node Port: %d \n", mainNode -> nodeAddr, mainNode -> nodePort);
	struct ripEntry* currEntry = table ->ripEntries;
	while(currEntry != NULL){
		printf("Destination: %s NextHopAddr: %s:%d VIP: %s \n",currEntry -> destVIP, currEntry -> nextHop -> rnAddr, currEntry -> nextHop -> rnPort, currEntry -> nextHop -> vipDest);
		printf("Cost %d\n", currEntry -> cost);
		currEntry = currEntry -> next;
	}  
}
void* reader(void* data){
	struct returnInfo* allInfo = (struct returnInfo*) data;
	struct nodeInfo* mainNode = allInfo -> mainNode;
	struct interface* intList = allInfo -> interfaceList;
	printf("Read thread run \n");
	return NULL;
}
void* listener(void* data){
	struct returnInfo* allInfo = (struct returnInfo*) data;
        struct nodeInfo* mainNode= allInfo -> mainNode;
        struct interface* intList = allInfo -> interfaceList;
	printf("Listener thread run \n");
	return NULL;
}
char* getThirdArg(char* string, char delimiter){
	int i = 0;
	int count = 0;
	while(string[i] != '\0'){
		if(string [i] == delimiter){
			count ++;
		}
		if(count >= 2){
			return &string[i];
		}
		i++;
	}
	return string;
}
int main(int argc, char ** argv){
	struct returnInfo* returnData = parser(argc, argv);
	struct ripTable* mainTable =(struct ripTable*) malloc(sizeof(ripTable));
	initializeTable(returnData -> mainNode, returnData -> interfaceList, mainTable);
	printTable(mainTable);
	testUpdate(mainTable);
	printTable(mainTable);	
	prepareUpdateData(mainTable, mainTable -> intList);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_t* readThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_t* listenThread = (pthread_t*) malloc(sizeof(pthread_t));
	int rc = 0;
	//Start listening/dealing with updating
	if(rc = pthread_create(readThread, &attr, reader, (void*)returnData)){
		printf("thread creation error %d\n", rc);	
	}
	//send initial request for data
	if(rc = pthread_create(listenThread, &attr, listener,(void*)returnData)){
                printf("thread creation error%d\n", rc);
        }
	//triggered updates
	 if(rc = pthread_create(listenThread, &attr, listener,(void*)returnData)){
                printf("thread creation error%d\n", rc);
        }
	//Check if threads are expired
	 if(rc = pthread_create(listenThread, &attr, listener,(void*)returnData)){
                printf("thread creation error%d\n", rc);
        }


char buffer[100];
while(1){
	char * origBuffer = malloc(sizeof(char) * 256);
	fgets( buffer, 256, stdin);
	strcpy(origBuffer, buffer);
//	printf("buffer %s", buffer);
	char* command = strtok(buffer, " ");
	char* firstArgument = strtok(NULL, " "); 
//	printf("command %s %d end \n", buffer, strcmp(command, "ifconfig"));
	if(!strcmp(origBuffer, "ifconfig\n")){
		struct interface* currInt = returnData -> interfaceList;
		while(currInt != NULL){
			printf("%d \t %s \t", currInt -> interId, currInt -> vipSource);
			if(currInt -> upDown) printf("up");
			else printf("down");
			currInt = currInt -> next;
			printf("\n");
		}		
	}
	if(!strcmp(origBuffer, "routes\n")){printf("routes not implemented");}
//	printf("firstArg %s \n",firstArgument);
	 if(!strcmp(command,"up")){
		int i = 0;
		int arg = atoi(firstArgument);
		struct interface* currInt = returnData -> interfaceList;
		for(i = 0; i <arg-1; i ++){
			if(currInt -> next == NULL) printf("Node doesn't exist!");
			currInt = currInt -> next;
		}
		currInt -> upDown = 1;
		printf("Node %d up!\n", arg);
	}
	if(!strcmp(command,"down")){
		int i = 0;
                int arg = atoi(firstArgument);
                struct interface* currInt = returnData -> interfaceList;
		for(i = 0; i <arg-1; i ++){
			if(currInt -> next == NULL) printf("Node doesn't exist!");
                        currInt = currInt -> next;
                }
                currInt -> upDown = 0;
		//Need to launch thread to notify others about it being down
		printf("Node %d down!\n", arg);
		prepareUpdateData(mainTable, mainTable -> intList);
	}
	if(!strcmp(command, "send")){
//		printf("origBuffer %s \n", origBuffer);
		char* ipString = firstArgument;
		char* messagePayload = getThirdArg(origBuffer, ' ');
		printf("payload:%s \n", messagePayload);
		struct interface* nextHop = getRouteByDestVIP(ipString,mainTable);
		struct nodeInfo* nextHopInfo = (struct nodeInfo*)malloc(sizeof(struct nodeInfo));
		nextHopInfo -> nodeAddr = nextHop -> rnAddr;
		nextHopInfo -> nodePort = nextHop -> rnPort;
		printf("NEXT HOP: %s:%d", nextHopInfo -> nodeAddr,nextHopInfo -> nodePort);
		//launch thread to send data -- need to include route data
		 if(rc = pthread_create(listenThread, &attr, listener,(void*)returnData)){
                printf("thread creation error%d\n", rc);
        }

	}

}


//pthread_join(*readThread,NULL);
//pthread_join(*listenThread, NULL);
}
