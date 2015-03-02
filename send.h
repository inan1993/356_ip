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
#include "ipHeader.h"
#ifndef RIPTABLE_H
	#define RIPTABLE_H
#endif
//uint16_t command;
//uint16_t num_entries;
//struct {
//uint32_t cost;
//uint32_t address;
//} entries[num_entries];


#define MAX_MSG_LENGTH 1400


int sender(void* msg, char*source_addVIP, char* dest_addr, uint16_t port, char*dest_addVIP,  int flag, int size_of_payload, int TTL){
	int sock;
 	struct sockaddr_in destination_addr;
 	if ((sock = socket(AF_INET, SOCK_DGRAM/* use UDP */, 0)) < 0) {
 		perror("Create socket error:");
 		return 1;
 	}
 	printf("Socket created\n");

 	destination_addr.sin_addr.s_addr = inet_addr(dest_addr);
 	destination_addr.sin_family = AF_INET;
 	destination_addr.sin_port = htons(port);
 
 	if (connect(sock, (struct sockaddr*)&destination_addr, sizeof(destination_addr)) < 0) {
 		perror("Connect error:");
 		return 1;
 	}
 	printf("Connected to node %s:%d\n", dest_addr, port);
 	// printf("length of buffer %lu\n",sizeof(*msg));						// FIX???
 
 	void *stream;
 	stream = malloc(MAX_MSG_LENGTH);

	// int size_of_payload = 2;
 	ip_packet ip = ipHeader((char*)msg, source_addVIP, dest_addVIP, size_of_payload, flag, TTL);			//CASTED TO CHAR*
 	printf("size of IP struct = %lu\n", sizeof(ip));
 	printf("size of stream = %lu\n", sizeof(stream));
 	
 	memcpy(stream, &ip,sizeof(ip));
 	if(flag==1|flag==2){					//RIP update	
		memcpy(stream+sizeof(ip), &flag,sizeof(int));
		memcpy(stream+sizeof(ip)+sizeof(int), &size_of_payload,sizeof(int));
		memcpy(stream+sizeof(ip)+(2*sizeof(int)), (void*)msg,sizeof(struct ripUpdate)*size_of_payload);
 	}else if (flag==3){
 		memcpy(stream+sizeof(ip), (void*)msg,size_of_payload);
 	}
 	
 	// printf("Stream %s\n", (char*)stream+sizeof(ip));
 
     if (sendto(sock, (void*)stream,MAX_MSG_LENGTH, 0,0,0) < 0) {
     	perror("Send error:");
     	return 1;
     }
 	free(stream);
    close(sock);
    printf("Packet sent\n\n");
    return 0;
 }

void testUpdate(char* dest, int cost, char* source, struct ripUpdate* testUpdate){
        

    testUpdate -> destVIP =inet_addr(dest);
    printf("Long------------- %u\n", inet_addr(dest));
    testUpdate -> cost = cost;
    testUpdate -> sourceVIP = inet_addr(source);
// 		updateTable(testUpdate,mainTable);
//      printTable(mainTable);
}

// int intitializer(int argc, char ** argv){
// 	char *str = "127.0.0.1";
// 	uint16_t port = 1700;
// 	char* msg = "Ipsum lorem baconnn";
// 	int flag = 0; //					flag for detecting if datagram = 1 or RIP = 0
	

// 	struct ripUpdate* currUpdate = (struct ripUpdate*)malloc(sizeof(struct ripUpdate)*2);
// 	// struct ripUpdate* testUpdate1 = (struct ripUpdate*)malloc(sizeof(struct ripUpdate));
// 	// struct ripUpdate* testUpdate2 = (struct ripUpdate*)malloc(sizeof(struct ripUpdate));
// 	// testUpdate("123.456.789.000", 3, "000.987.654.321", testUpdate1);
// 	// testUpdate("123.456.789.000", 8, "111.111.255.255", testUpdate2);
// 	testUpdate("123.456.789.000", 3, "000.987.654.321", &currUpdate[0]);
// 	testUpdate("123.456.789.000", 8, "111.111.255.255", &currUpdate[1]);

// 	int c = sender((char*)currUpdate, str, port, str, flag);

// }




