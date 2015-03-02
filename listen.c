#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "ipHeader.h"
#include "ripTable.h"
//uint16_t command;
//uint16_t num_entries;
//struct {
//uint32_t cost;
//uint32_t address;
//} entries[num_entries];

#define BUFF_SIZE 1401

int listening(char * addr, uint16_t port){
	int sock;
	struct sockaddr_in my_addr;
	if ((sock = socket(AF_INET, SOCK_DGRAM/* use UDP */, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}
	printf("Socket created\n");

	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = inet_addr(addr);			
	my_addr.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
		perror("Binding socket to local address error:");
		return 1;
	}


	char buf [BUFF_SIZE];
	// void* buf = malloc(BUFF_SIZE);

//Code from the internet
	struct sockaddr_storage src_addr;
	socklen_t src_addr_len=sizeof(src_addr);
	while(1){

		ssize_t count=recvfrom(sock,&buf,BUFF_SIZE,0,(struct sockaddr*)&src_addr,&src_addr_len);  // return number of bytes received
		struct sockaddr_in *sin = (struct sockaddr_in*)&src_addr;
		char *IP = (char*)&sin->sin_addr.s_addr;
		printf("Source address-----%d.%d.%d.%d\n", IP[0], IP[1], IP[2], IP[3]);

		ip_packet ip;
		memcpy(&ip,buf,sizeof(ip));
		// char* msg = (char*)(buf+sizeof(ip));

		if (count==-1) {
			printf("ERORR %s",strerror(errno));
		} else if (count==sizeof(buf)) {
			printf("datagram too large for buffer: truncated\n");
			// printf("count = %zi\n",count);
			// printf("checksum=%hu\n", ip.ip_header.ip_sum);
		} else {
			printf("count = %zi \n",count);
			printf("TOS=%d\n",ip.ip_header.ip_tos);			// handle_dgram method
			// printf("Payload=%s\n", msg);
			if(ip.ip_header.ip_tos==0){
				int req_update;
				memcpy(&req_update, buf+sizeof(ip),sizeof(int));
				if(req_update==1){
					printf("REQUEST FOR UPDATE...\n");
				}else if(req_update==0){
					printf("Updating ripTable\n");
					int sizeof_update;
					memcpy(&sizeof_update, buf+sizeof(ip)+sizeof(int),sizeof(int));
					printf("sizeofUPDATE-%d\n",sizeof_update);

					struct ripUpdate* currUpdate = (struct ripUpdate*)malloc(sizeof(struct ripUpdate)*sizeof_update);
					memcpy(&currUpdate, buf+(sizeof(ip)+2*sizeof(int)),sizeof(struct ripUpdate)*sizeof_update+1);


				}
			}else if(ip.ip_header.ip_tos==1){
				char* msg;
				memcpy(&msg, buf+sizeof(ip), ip.ip_header.ip_len-sizeof(struct ip));
				printf("Datagram - %s\n", (char*)(buf+sizeof(ip)));
			}
		}
	}
	return 0;
}
// 
int main(int argc, char ** argv){
	char *listen_on = "127.0.0.1";
	uint16_t port = 1700;
	int c = listening(listen_on, port);
}
