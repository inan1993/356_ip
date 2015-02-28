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

typedef struct ip_packet
{	
	ip_t ip_header;
	char* payload;
}ip_packet;


void ipHeader(char* payload){

	ip_packet ip;
	ip.ip_header.ip_len = 12;
	ip.payload = payload;

	printf("Length = %lu\n Payload = %s",ip.ip_header.ip_len,ip.payload);
	return;
}


int main(int argc, char** argv){

	char* payload = "WOLFRAM";
	ipHeader(payload);

}

//  typedef struct ip
// 00081 {
// 00082 #if __BYTE_ORDER__ == __LITTLE_ENDIAN__
// 00083         uint8_t ip_hdr_len:4;   //!< The header length.
// 00084         uint8_t ip_version:4;   //!< The IP version.
// 00085 #else
// 00086         uint8_t ip_version:4;   //!< The IP version.
// 00087         uint8_t ip_hdr_len:4;   //!< The IP header length.
// 00088 #endif
// 00089         //! Type of Service.
// 00090         uint8_t ip_tos;
// 00091         //! IP packet length (both data and header).
// 00092         uint16_t ip_len;
// 00093 
// 00094         //! Identification.
// 00095         uint16_t ip_id;
// 00096         //! Fragment offset.
// 00097         uint16_t ip_off;
// 00098 
// 00099         //! Time To Live.
// 00100         uint8_t ip_ttl;
// 00101         //! The type of the upper-level protocol.
// 00102         uint8_t ip_proto;
// 00103         //! IP header checksum.
// 00104         uint16_t ip_chk;
// 00105 
// 00106         //! IP source address (in network format).
// 00107         uint32_t ip_src;
// 00108         //! IP destination address (in network format).
// 00109         uint32_t ip_dst;
// 00110 } __attribute__ ((packed)) ip_t;