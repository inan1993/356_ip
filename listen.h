#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#ifndef THREADMETHODS_H
#define THREADMETHODS_H
#include "threadMethods.h"
#endif
#ifndef SEND_H
#define SEND_H
#include "send.h"
#endif
//uint16_t command;
//uint16_t num_entries;
//struct {
//uint32_t cost;
//uint32_t address;
//} entries[num_entries];

#define BUFF_SIZE 1401
struct user{
        struct ripTable* mainTable;
        int ttl;
        struct sendData* buffer;
        char * destVIP;
};
void* sendUpdateCopy(void* data){
         struct ripTable* init = (struct ripTable*) data;
         struct interface * currInt = init -> intList;
         while(currInt != NULL){
                        if(currInt -> upDown == 0){
                                struct sendData* info = prepareUpdateData(init, currInt, 0);
                                sender((void*)(info -> buffer),currInt -> vipSource, currInt -> rnAddr, currInt -> rnPort, currInt -> vipDest, info -> flag, info -> size, 15);
  //                              printf(" sent update!\n");
                        }
                        currInt = currInt -> next;
}
}
void* sendUserDataCopy(void* data){
        struct user* init = (struct user*)data;
        char* destVIP = init -> destVIP;
        struct interface* currInt = init -> mainTable -> intList;
        struct sendData* info = init -> buffer;
        while(currInt != NULL){
                if(addrToNumber(currInt -> vipSource) == addrToNumber(destVIP)){
                        printf("Sent Data: %s \n", init -> buffer -> buffer);
                        return;
                }
                currInt = currInt -> next;
        }
        struct interface* route =  getRouteByDestVIP(destVIP,init ->  mainTable);
        //decrement TTL
//        send((void*)init -> buffer, init -> mainTable -> mainNode -> nodeAddr, interface -> rnPort, interface -> vipDest, sendData -> flag, sendData -> size, destVIP);
        sender((void*)(info -> buffer),currInt -> vipSource, currInt -> rnAddr, currInt -> rnPort, currInt -> vipDest, info -> flag, info -> size, 15);
     printf("sent user data %s \n", init->buffer->buffer);
}

int listening(char * addr, uint16_t port, struct ripTable* mainTable){
	int sock;
	struct sockaddr_in my_addr;
	if ((sock = socket(AF_INET, SOCK_DGRAM/* use UDP */, 0)) < 0) {
	perror("Create socket error:");
	return 1;
	}
	printf("Socket created: %s %d\n", addr, port);

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
//		printf("waiting");
		ssize_t count=recvfrom(sock,&buf,BUFF_SIZE,0,(struct sockaddr*)&src_addr,&src_addr_len);  // return number of bytes received
//		printf("received \n");
		struct sockaddr_in *sin = (struct sockaddr_in*)&src_addr;
		char *IP = (char*)&sin->sin_addr.s_addr;
//		printf("Source address-----%d.%d.%d.%d\n", IP[0], IP[1], IP[2], IP[3]);

		ip_packet ip;
		memcpy(&ip,buf,sizeof(ip));
//		printf("\nIP headr length\t\t- %i\n", ip.ip_header.ip_hl);
//		printf("IP version\t\t- %i\n", ip.ip_header.ip_v);
//		printf("IP tos\t\t\t- %i\n", ip.ip_header.ip_tos);
//		printf("IP TTL\t\t\t- %i\n", ip.ip_header.ip_ttl);
//		printf("IP length of packet\t- %i\n", ip.ip_header.ip_len);
//		printf("IP protocol\t\t- %i\n\n", ip.ip_header.ip_p);
//		printf("IP checksum\t\t- %i\n\n", ip.ip_header.ip_sum);

//		printf("IP src\t\t- %s\n\n", inet_ntoa(ip.ip_header.ip_src));
//		printf("IP dest\t\t- %s\n\n", inet_ntoa(ip.ip_header.ip_dst));

		// char* msg = (char*)(buf+sizeof(ip));

		if (count==-1) {
			printf("ERORR %s",strerror(errno));
		} else 
		if (count==sizeof(buf)) {
			printf("datagram too large for buffer: truncated\n");
		} else {
//			printf("count = %zi \n",count);
//			printf("TOS=%d\n",ip.ip_header.ip_tos);			// handle_dgram method
			// printf("Payload=%s\n", msg);
			char* src_VIP = inet_ntoa(ip.ip_header.ip_src);
			char* my_VIP = inet_ntoa(ip.ip_header.ip_dst);

//			printf("Packet from ----- %s\n", src_VIP);



			if(ip.ip_header.ip_tos==1|ip.ip_header.ip_tos==2){
				char* stream;
				stream = (char*)(buf+sizeof(ip)+(2*sizeof(int)));
				int checksum = ip_sum(stream,(sizeof(struct ip)));
				if(checksum == ip.ip_header.ip_sum){}else{printf("check failed \n");continue;}
//				printf("%d________ %d", checksum, ip.ip_header.ip_sum);
				
				//	struct interface* intOfDest = getInterfaceFromNextHopVIP(mainTable, src_VIP);
			//	printf("goes down here: %s  %s \n", my_VIP, src_VIP);
				struct interface* intOfDest = getByInterfaceSrc(mainTable, my_VIP);
				if(intOfDest -> upDown == 100){
				//	printf("interface %d down: packet dropped \n", intOfDest -> interId);
					continue;
				}
				int req_update;
				memcpy(&req_update, buf+sizeof(ip),sizeof(int));
		//		printf("reqUpdate: %d \n", req_update);
				if(ip.ip_header.ip_tos==2){								// 2 = REQUEST for UPDATE
					printf("Request for Update\n");
					// New Array of RIP_Updates
				//	void* msg = prepareUpdateData(mainTable, intOfDest, 0/*getInterfaceFromNextHopVIP(mainTable, src_VIP)*/);
				 pthread_attr_t attr;
			        pthread_attr_init(&attr);
			        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

				 pthread_t* requestThread = (pthread_t*) malloc(sizeof(pthread_t));
			        int rc = 0;
      				  //Start listening/dealing with updating
			        if(rc = pthread_create(requestThread, &attr, sendUpdateCopy, (void*)mainTable)){
			                printf("thread creation error %d\n", rc);
			        }

				//	if(sender(msg, my_VIP, intOfDest->rnAddr,intOfDest->rnPort, src_VIP, 1, getTableLength(mainTable),16)!=0){
				//		printf("ERROR sending the requested update.\n");
				//	}
//					printf("Update Sent\n");

				}else if(req_update==1){					// 1 = RECEIVING UPDATE
//					printf("Updating ripTable\n");
					int sizeof_update;
					memcpy(&sizeof_update, buf+sizeof(ip)+sizeof(int),sizeof(int));
//					printf("sizeofUPDATE-%d\n",sizeof_update);
					struct ripUpdate* currUpdate = (struct ripUpdate*)malloc(sizeof(struct ripUpdate)*sizeof_update);
					currUpdate = (struct ripUpdate*)(buf+(sizeof(ip)+2*sizeof(int)));
				// printf("DEST - %lu\n",currUpdate[1].destVIP);
					// Updating table with each ripUpdate
					//	if()
						int i = 0;
						for ( i=0; i<sizeof_update;i++){
//						printf("Update no.-%d\n", i+1);
						updateTable(&currUpdate[i], mainTable);
					}

							// int sizeof_update;
							// memcpy(&sizeof_update, buf+sizeof(ip)+sizeof(int),sizeof(int));
							// printf("size of UPDATE-%d\n",sizeof_update);
							// struct ripUpdate* currUpdate = (struct ripUpdate*)malloc(sizeof(struct ripUpdate)*sizeof_update);
							// currUpdate = (struct ripUpdate*)(buf+(sizeof(ip)+2*sizeof(int)));
							// for (int i=0; i<sizeof_update;i++){
							// 	printf("Update no.-%d\n", i+1);
							// 	printf("DEST - %lu\n",currUpdate[i].destVIP);
							// 	printf("Cost - %lu\n",currUpdate[i].cost);
							// 	printf("NextHop - %lu\n",currUpdate[i].sourceVIP);
							// }
				}
				}else 
				if(ip.ip_header.ip_tos==3){
					char* stream;
					stream = (char*)(buf+sizeof(ip));
					int checksum = ip_sum(stream,(sizeof(struct ip)));
					if(checksum == ip.ip_header.ip_sum){printf("Checksum match: %d\n",checksum);}else{continue;}

					// remember to change IP header with new fields - TTL etc

					int ttl = ip.ip_header.ip_ttl;
					printf("Forwarding packet - TTL = %d\n",ttl);
					int length = ip.ip_header.ip_len-sizeof(struct ip);
					struct sendData* buff = malloc(sizeof(int)*2+sizeof(char)*length);
						buff->flag = ip.ip_header.ip_tos;   // Should be 3
						buff->size = length;
						strcpy(buff->buffer, buf+sizeof(ip));

						struct user* forwardTo = malloc(sizeof(struct user*));
						forwardTo->ttl = ip.ip_header.ip_ttl;
						forwardTo->mainTable = mainTable;
						forwardTo->destVIP =  inet_ntoa(ip.ip_header.ip_dst);
						forwardTo->buffer = buff;

						pthread_attr_t attr;
						pthread_attr_init(&attr);
						pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
						pthread_t* forwardThread = (pthread_t *)malloc(sizeof(pthread_t));
						int rc = 0;
						//Forward updates
					if(rc == pthread_create(forwardThread, &attr, sendUserDataCopy, (void*)forwardTo)){
						printf("thread creation error %d\n", rc);	
					}
				}
			}
		}
		return 0;
		}

//int main(int argc, char ** argv){
//	char *listen_on = "127.0.0.1";
//	uint16_t port = 1700;
//	struct ripTable* mainTable;
//	int c = listening(listen_on, port, mainTable);
//}
