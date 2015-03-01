#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
//uint16_t command;
//uint16_t num_entries;
//struct {
//uint32_t cost;
//uint32_t address;
//} entries[num_entries];

struct nodeInfo
{
	char* nodeAddr;
	int nodePort;
} nodeInfo;

struct interface{
	int interId;
	char* rnAddr;
	int rnPort;
	char * vipSource;
	char * vipDest;
	int upDown;
	struct interface* next;
	struct interface* prev;
}interface;

struct returnInfo{
	struct nodeInfo* mainNode ;
	struct interface* interfaceList;
} returnInfo;

void printParser(struct nodeInfo *nodeInformation, struct interface* listStart){
	printf("Local Node address: %s \n", nodeInformation -> nodeAddr);
	printf("Local Node Port: %i \n\n", nodeInformation -> nodePort);
	struct interface * currInterface = listStart;
	while(currInterface != NULL){
		printf("Interface ID: %i \n", currInterface -> interId);
		printf("Remote Node addr: %s \n", currInterface -> rnAddr);
		printf("Remote Node Port: %i \n", currInterface -> rnPort);
		printf("Interface vip: %s \n", currInterface -> vipSource);
		printf("Interface Destination vip: %s\n\n ", currInterface -> vipDest);
		currInterface = currInterface -> next;
	}
}

struct returnInfo* parser(int argc, char ** argv){
	struct nodeInfo* nodeInformation =(struct nodeInfo*) malloc(sizeof(nodeInfo));

	struct interface* interfaceList = (struct interface*) malloc(sizeof(interface)); 
	interfaceList->prev = NULL; 
	interfaceList->next = NULL;

	
	char* filename = argv[1];
	FILE* fd = fopen(filename, "rw");
	int c = 0;
	char* word = malloc(sizeof(char) * 256);
	int lineCounter = 0, wordCounter = 0;
	struct interface* currInt = interfaceList;
	while(!feof(fd)){
		fscanf(fd,"%s", word);
		if(feof(fd))
			break;
		printf("WORD = %s\n",word);
		if(word[0] == '\n'){
			printf("new!!");
		}

		//	Initialize local node  - First Line
		if(lineCounter == 0){
			char* temp = (char*)malloc(sizeof(char)*100);
			strcpy(temp,strtok(word, ":"));
			nodeInformation->nodeAddr = temp;	
			printf("nodeaddr %s\n",nodeInformation->nodeAddr);	
			unsigned long address = inet_addr(word);
					// printf("addr = %lu", address);
			int port = atoi(strtok(NULL, ":"));					// NULL ???
			nodeInformation->nodePort = port;
					// printf("port = %i\n", port);
			lineCounter ++;	
		}

		//Initialize first interface + Other nodes and interfaces.
		else	if(lineCounter > 0 && c!=EOF){
			struct interface* nextInterface = (struct interface*)malloc(sizeof(interface));
			nextInterface->next = NULL;
			currInt->next = nextInterface;
			currInt->next->prev = currInt;
			currInt->interId = lineCounter;
			currInt -> upDown = 0;
			if(wordCounter == 0){
				char* words = strtok(word,":");
				char* wordy = (char*) malloc(sizeof(char) * 100);
				strcpy(wordy, words);
				if(!strcmp(wordy, "localhost")){
					printf("YAY!\n");
					currInt->rnAddr = "127.0.0.1";
				}
				else{
					currInt->rnAddr = wordy;
				}
				currInt->rnPort = atoi( strtok(NULL, ":"));
				wordCounter ++;
			}
			else if(wordCounter == 1){
				char* wordy = (char*) malloc(sizeof(char) * 100);
				strcpy(wordy, word);
				currInt->vipSource = wordy;
				wordCounter ++;
			}
			else if(wordCounter == 2){
				//	unsigned long destVip = inet_addr(word);
				char* wordy = (char*) malloc(sizeof(char) * 100);
				strcpy(wordy, word);
				currInt->vipDest = wordy;
				lineCounter ++;
				wordCounter = 0;
				currInt = currInt->next;
			}
			else{
				printf("ERROR\n");
			}
		}
		
	}	
	printf("-----END-----\n\n");
	currInt->prev->next = NULL;
	struct returnInfo* returnValue = (struct returnInfo*) malloc(sizeof(struct returnInfo));
	returnValue->mainNode = nodeInformation;
	returnValue -> interfaceList = interfaceList;
	printParser(nodeInformation,interfaceList);
	return returnValue;
}




