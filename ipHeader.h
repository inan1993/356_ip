#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <netinet/ip.h>
// #include <net/ip.h>
#include <inttypes.h>				


typedef struct ip_Packet
{	
	struct ip ip_header;
	// char payload [PACKET_LEN];
}ip_packet;


int ip_sum(char* packet, int n) {
  uint16_t *p = (uint16_t*)packet;
  uint16_t answer;
  long sum = 0;
  uint16_t odd_byte = 0;

  while (n > 1) {
    sum += *p++;
    n -= 2;
  }

  /* mop up an odd byte, if necessary */
  if (n == 1) {
    *(uint8_t*)(&odd_byte) = *(uint8_t*)p;
    sum += odd_byte;
  }

  sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
  sum += (sum >> 16);           /* add carry */
  answer = ~sum;                /* ones-complement, truncate*/
  return answer;
}

ip_packet ipHeader(char* payload, char* src, char* dest, unsigned int size_of_payload, int flag, int TTL){

	ip_packet ip;						// creating ip Headers
	ip.ip_header.ip_hl 		= (unsigned int)sizeof(struct ip);				// Stores the last 4 bits only		// ACTUAL SIZE = 20bytes
	ip.ip_header.ip_v 		= 4;		// IPv4
	ip.ip_header.ip_tos 	= flag;		// Type of Service ????
	ip.ip_header.ip_len 	=sizeof(struct ip)+size_of_payload;
	ip.ip_header.ip_id 		=0;												//EXTRA CREDIT
	ip.ip_header.ip_off 	=0;												//EXTRA CREDIT
// #define	IP_RF 0x8000			/* reserved fragment flag */
// #define	IP_DF 0x4000			/* dont fragment flag */
// #define	IP_MF 0x2000			/* more fragments flag */
// #define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
	ip.ip_header.ip_p 		= 17; 		// UDP
	ip.ip_header.ip_ttl		= TTL;		// Max Time to live = Infinity
	ip.ip_header.ip_sum		= ip_sum(payload, (sizeof(struct ip)));
	inet_aton(src, &ip.ip_header.ip_src);
	inet_aton(dest, &ip.ip_header.ip_dst);
/*
	printf("IP headr length\t\t- %i\n", ip.ip_header.ip_hl);
	printf("IP version\t\t- %i\n", ip.ip_header.ip_v);
	printf("IP tos\t\t\t- %i\n", ip.ip_header.ip_tos);
	printf("IP TTL\t\t\t- %i\n", ip.ip_header.ip_ttl);
	printf("IP length of packet\t- %i\n", ip.ip_header.ip_len);
	printf("IP protocol\t\t- %i\n\n", ip.ip_header.ip_p);
	printf("IP checksum\t\t- %i\n\n", ip.ip_header.ip_sum);

	printf("IP src\t\t- %s\n\n", inet_ntoa(ip.ip_header.ip_src));
	printf("IP dest\t\t- %s\n\n", inet_ntoa(ip.ip_header.ip_dst));
	

	printf("LengthOfPayload = %lu\nPayload = %s\n",size_of_payload, payload);
	*/return ip;
}
