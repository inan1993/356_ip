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
//#include "listen.h"
//uint16_t command;
//uint16_t num_entries;
//struct {
//uint32_t cost;
//uint32_t address;
//} entries[num_entries];

struct ripEntry{
	char destVIP[30];
	struct interface nextHop;
	int cost;
	time_t updateTime;
	bool upDown;
	struct ripEntry* next;
	struct ripEntry* prev;
}ripEntry;

struct ripTable{
	struct interface interfaceList;
	struct nodeInfo mainNode; 
}ripTable;
struct threadInit{
	struct nodeInfo mainNode;
	
};
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
int main(int argc, char ** argv){
	struct returnInfo* returnData = parser(argc, argv);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_t* readThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_t* listenThread = (pthread_t*) malloc(sizeof(pthread_t));
	int rc = 0;
	if(rc = pthread_create(readThread, &attr, reader, (void*)returnData)){
		printf("thread creation error %d\n", rc);	
	}
	if(rc = pthread_create(listenThread, &attr, listener,(void*)returnData)){
                printf("thread creation error%d\n", rc);
        }
char buffer[256]; 
while(1){
	scanf("%s", buffer);
//	printf("%s \n", buffer);
	if(!strcmp(buffer, "ifconfig")){
		printf("Yay! config!");
	}
}


//pthread_join(*readThread,NULL);
//pthread_join(*listenThread, NULL);
}
