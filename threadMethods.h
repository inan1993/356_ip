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
struct update{
	struct ripTable* mainTable;
	struct ripUpdate* upList;
	struct interface* currInt;
};
struct 
struct
void* triggeredUpdates(void* data){
	something = ripTable mainTable;
	struct interface* currInt = mainTable -> interfaces;
	while(1){
		while(currInt != NULL){
			struct ripUpdate* = prepareUpdateData(mainTable, currInt);
			//send prepared data;
			printf("sent data");
			currInt = currInt -> next;
			sleep(5);
		}
	}
}

void* sendUpdate(void* data){
	while(currInt != NULL){
		struct ripUpdate* = prepareUpdateData(mainTable, currInt);
		//send these updates
		currInt = currInt -> next;
	}
}

void* listen(void* data){

}
//could be combined with  triggered updates
void* checkTimeout(void* data){
	
}

void* sendUserData(void* data){

}

void* sendDataRequest(void* data){
	
}
