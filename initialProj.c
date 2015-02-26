#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int server(uint16_t port);
int client(const char * addr, uint16_t port);

#define MAX_MSG_LENGTH (512)
#define MAX_BACK_LOG (5)

int main(int argc, char ** argv)
{
	if (argc < 3) {
		printf("Command should be: myprog s <port> or myprog c <port> <address>\n");
		return 1;
	}
	int port = atoi(argv[2]);
	if (port < 1024 || port > 65535) {
		printf("Port number should be equal to or larger than 1024 and smaller than 65535\n");
		return 1;
	}
	if (argv[1][0] == 'c') {
		if(argv[3]==NULL){
			printf("NO IP address is given\n");
			return 1;
		}
		return client(argv[3], port);
	} else if (argv[1][0] == 's') {
		return server(port);
	} else {
		printf("unknown command type %s\nCommand should be: myprog s <port> or myprog c <port> <address>", argv[1]);
		return 1;
	}
	return 0;
}

int client(const char * addr, uint16_t port)
{
	int sock;
	struct sockaddr_in server_addr;
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

int server(uint16_t port)
{
 int sock, receive;
	struct sockaddr_in server_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Create socket error:");
		return 1;
	}
	 server_addr.sin_family = AF_INET;
	 server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	 server_addr.sin_port = htons(port);

 	bind (sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	while(listen(sock, 10) != 0){
		printf("Listen error");
	}

	while(1){
		receive = accept(sock, NULL,NULL);
		int state = fork();
		if(state == 0){
			break;
		}
	}	
	printf("Server started %d\n", port);
	char msg[MAX_MSG_LENGTH+1], reply[MAX_MSG_LENGTH*3+3];
		memset(reply, 0, sizeof(reply));
	int recv_len = 0;
	while (1) {
		recv_len = read(receive, msg, MAX_MSG_LENGTH);
		if(recv_len == 0){
			return 1;
                }
		msg[MAX_MSG_LENGTH]  =(char) NULL;
		strcat(reply, msg);
		strcat(reply,msg);
		strcat(reply,msg);
		if (recv_len < 0) {
			return 1;
		}
		if(recv_len == 0){
			
			while(listen(sock, 10) < 0){
			}
        		receive = accept(sock, NULL,NULL);
		}	
		 send(receive, reply, MAX_MSG_LENGTH*3,0);//{
		
		reply[recv_len] = 0;
		memset(reply, 0, sizeof(reply));
	}
	printf("Closing socket");
	close(sock);
	return 0;
}

