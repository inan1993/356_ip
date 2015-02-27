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
	struct interface* next;
	struct interface* prev;
}interface;

void printParser(struct nodeInfo *nodeInformation, struct interface* listStart){
	printf("Local Node address: WRONG!!!!!!     %s \n", nodeInformation->nodeAddr);
	printf("Local Node Port: %i \n\n", nodeInformation->nodePort);
	struct interface * currInterface = listStart;
	while(currInterface != NULL){
		printf("Interface ID: %i \n", currInterface->interId);
		printf("Remote Node addr: %s \n", currInterface->rnAddr);
		printf("Remote Node Port: %i \n", currInterface->rnPort);
		printf("Interface Local vip: %s \n", currInterface->vipSource);
		printf("Interface Destination vip: %s\n\n", currInterface->vipDest);
		currInterface = currInterface->next;
	}
}

int main(int argc, char ** argv){
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
		// if(word[0] == '\n'){
		// 	printf("new!!");
		// }

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
					printf("port = %i\n", port);
			lineCounter ++;	
		}

		//Initialize first interface + Other nodes and interfaces.
		else	if(lineCounter > 0 && c!=EOF){
			struct interface* nextInterface = (struct interface*)malloc(sizeof(interface));
			nextInterface->next = NULL;
			currInt->next = nextInterface;
			currInt->next->prev = currInt;
			currInt->interId = lineCounter;

			if(wordCounter == 0){
				char* words = strtok(word,":");
				char* wordy = (char*) malloc(sizeof(char) * 100);
				strcpy(wordy, words);
				if(!strcmp(wordy, "localhost")){
					printf("YAY!\n");
					currInt->rnAddr = "127.0.0.0";
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
	printParser(nodeInformation,interfaceList);


// Thread for receiving



// Thread for sending
}

int listening(const char * addr, uint16_t port)
{
	int sock;
	struct sockaddr_in my_addr;
	if ((sock = socket(AF_INET, SOCK_DGRAM/* use UDP */, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = htonl(addr);			// check if correct data type *addr ??
	my_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
		perror("Binding socket to local address error:");
		return 1;
	}

	char* buffer = malloc(sizeof(char)*1400);

//Code from the internet
	struct sockaddr_storage src_addr;
	socklen_t src_addr_len=sizeof(src_addr);

	ssize_t count=recvfrom(sock,buffer,sizeof(buffer),0,(struct sockaddr*)&src_addr,&src_addr_len);  // return number of bytes received
	if (count==-1) {
		die("%s",strerror(errno));
	} else if (count==sizeof(buffer)) {
		warn("datagram too large for buffer: truncated");
	} else {
		handle_datagram(buffer,count);
	}






	char msg[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH*3];
	memset(reply, 0, sizeof(reply));
	if ((sock = socket(AF_INET, SOCK_STREAM/* use tcp */, 0)) < 0) {
	perror("Create socket error:");
	return 1;
}

printf("Socket created\n");
server_addr.sin_addr.s_addr = inet_addr(addr);
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(port);

if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
	perror("Connect error:");
	return 1;
}

printf("Connected to server %s:%d\n", addr, port);

int recv_len = 0;
while (1) {
	fflush(stdin);
	printf("Enter message: \n");
	gets(msg);
	if (send(sock, msg, MAX_MSG_LENGTH, 0) < 0) {
		perror("Send error:");
		return 1;
	}
	recv_len = read(sock, reply, MAX_MSG_LENGTH*3);
	if (recv_len < 0) {
		perror("Recv error:");
		return 1;
	}
	reply[recv_len] = 0;
	printf("Server reply:\n %s\n", reply,msg);
	memset(reply, 0, sizeof(reply));
}
close(sock);
return 0;
}




