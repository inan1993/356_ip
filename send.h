#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <netinet/in.h>
//uint16_t command;
//uint16_t num_entries;
//struct {
//uint32_t cost;
//uint32_t address;
//} entries[num_entries];


#define MAX_MSG_LENGTH (1400)

int sender(const char * addr, uint16_t port, char* msgg, bool flag){
	int sock;
	struct sockaddr_in server_addr;
	char msg[MAX_MSG_LENGTH];
	if ((sock = socket(AF_INET, SOCK_DGRAM/* use UDP */, 0)) < 0) {
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
	printf("Connected to node %s:%d\n", addr, port);


	int recv_len = 0;

    if (send(sock, msgg, MAX_MSG_LENGTH, 0) < 0) {
    	perror("Send error:");
    	return 1;
    }

    close(sock);
    return 0;
}

int main(int argc, char ** argv){
	char *str = "127.0.0.1";
	uint16_t port = 1700;
	char* msgg = "Ipsum lorem baconnn";
	bool flag = true; //					flag for detecting if just ip or RIP
	int c = sender(str, port, msgg, flag);
}