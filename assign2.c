#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "parser.h"
//#include "listen.h"
//uint16_t command;
//uint16_t num_entries;
//struct {
//uint32_t cost;
//uint32_t address;
//} entries[num_entries];

void* reader(void* data){
	struct returnInfo* allInfo = (struct returnInfo*) data;
	struct nodeInfo* = allInfo -> mainNode;
	struct interface* intList = allInfo -> interface;
	printf("RRREEAADDD");
	return NULL;
}
void* listener(void* data){
	struct returnInfo* allInfo = (struct returnInfo*) data;
        struct nodeInfo* = allInfo -> mainNode;
        struct interface* intList = allInfo -> interface;
	printf("Listener");
	return NULL;
}
int main(int argc, char ** argv){
	struct returnInfo* returnData = parser(argc, argv);
	pthread_t* readThread = (pthread_t *)malloc(sizeof(pthread_t));
	pthread_t* listenThread = (pthread_t*) malloc(sizeof(pthread_t));
	if(pthread_create(readThread, NULL, reader, (void*)returnData)){
		printf("thread creation error\n");	
	}
	if(pthread_create(listenThread, NULL, listener,(void*)returnData)){
                printf("thread creation error\n");
        }



pthread_join(*readThread,NULL);
pthread_join(*listenThread, NULL);
}
