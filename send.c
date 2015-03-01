+#include <stdio.h>
+#include <string.h>
+#include <sys/socket.h>
+#include <sys/types.h>
+#include <arpa/inet.h>
+#include <unistd.h>
+#include <stdlib.h>
+#include <errno.h>
+#include <stdbool.h>
+#include <netinet/in.h>
+#include "ipHeader.h"
+//uint16_t command;
+//uint16_t num_entries;
+//struct {
+//uint32_t cost;
+//uint32_t address;
+//} entries[num_entries];
+
+
+#define MAX_MSG_LENGTH 1400
+
+int sender(const char * addr, uint16_t port, char* msgg, bool flag){
+	int sock;
+	struct sockaddr_in server_addr;
+	if ((sock = socket(AF_INET, SOCK_DGRAM/* use UDP */, 0)) < 0) {
+		perror("Create socket error:");
+		return 1;
+	}
+	printf("Socket created\n");
+	
+	server_addr.sin_addr.s_addr = inet_addr(addr);
+	server_addr.sin_family = AF_INET;
+	server_addr.sin_port = htons(port);
+
+	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
+		perror("Connect error:");
+		return 1;
+	}
+	printf("Connected to node %s:%d\n", addr, port);
+	// printf("length of buffer %lu\n",sizeof(*msgg));						// FIX???
+
+
+	ip_packet ip = ipHeader(msgg, addr, addr);
+	void *stream;
+	stream = malloc(MAX_MSG_LENGTH);
+	printf("size of IP struct = %lu\n", sizeof(ip));
+	printf("size of stream = %lu\n", sizeof(stream));
+	memcpy(stream, &ip,sizeof(ip));
+	memcpy(stream+sizeof(ip), (void*)msgg,100);
+	printf("Stream %s\n", (char*)stream+sizeof(ip));
+

+    if (sendto(sock, (void*)stream,MAX_MSG_LENGTH, 0,0,0) < 0) {
+    	perror("Send error:");
+    	return 1;
+    }
+
+    close(sock);
+    return 0;
+}
+
+int main(int argc, char ** argv){
+	char *destination = "127.0.0.1";
+	uint16_t port = 1700;
+	char msgg [100] = "0123456789 xxx";
+	bool flag = true; //					flag for detecting if just ip or RIP
+	int c = sender(destination, port, msgg, flag);
+}
