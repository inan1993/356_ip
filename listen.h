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


int main(int argc, char ** argv){
	listening('127.0.0.1', 1700);
}

int listening(const char * addr, uint16_t port){
	int sock;
	struct sockaddr_in my_addr;
	if ((sock = socket(AF_INET, SOCK_DGRAM/* use UDP */, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}
	printf("Socket created\n");

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
		printf("Buffer %s \n",buffer);			// handle_dgram method
	}
}


// int recv_len = 0;
// while (1) {
// 	fflush(stdin);
// 	printf("Enter message: \n");
// 	gets(msg);
// 	if (send(sock, msg, MAX_MSG_LENGTH, 0) < 0) {
// 		perror("Send error:");
// 		return 1;
// 	}
// 	recv_len = read(sock, reply, MAX_MSG_LENGTH*3);
// 	if (recv_len < 0) {
// 		perror("Recv error:");
// 		return 1;
// 	}
// 	reply[recv_len] = 0;
// 	printf("Server reply:\n %s\n", reply,msg);
// 	memset(reply, 0, sizeof(reply));
// }
// close(sock);
// return 0;
