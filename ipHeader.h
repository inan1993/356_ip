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
#define PACKET_LEN 140					//????


typedef struct ip_Packet
{	
	struct ip ip_header;
	// char payload [PACKET_LEN];
}ip_packet;



/************************************************************
 Checksum for Internet Protocol family headers (C Version)
 From ping examples in W.Richard Stevens "UNIX NETWORK PROGRAMMING" book.
************************************************************/

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
	
	// strcpy(ip.payload, payload);

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
	return ip;
}


// int main(int argc, char** argv){

// 	char payload[PACKET_LEN] = "WOLFRAMs";
// 	char* dest = "123.23.12.111";
// 	char* src = "123.23.12.111";
// 	ip_packet ip = ipHeader(payload, src, dest);
// }





/*
 * Structure of an internet header, naked of options.
 */
// struct ip
//   {
// #if __BYTE_ORDER == __LITTLE_ENDIAN
//     unsigned int ip_hl:4;		/* header length */
//     unsigned int ip_v:4;		/* version */
// #endif
// #if __BYTE_ORDER == __BIG_ENDIAN
//     unsigned int ip_v:4;		/* version */
//     unsigned int ip_hl:4;		/* header length */
// #endif
//     u_int8_t ip_tos;			/* type of service */
//     u_short ip_len;			/* total length */
//     u_short ip_id;			/* identification */
//     u_short ip_off;			/* fragment offset field */
// #define	IP_RF 0x8000			/* reserved fragment flag */
// #define	IP_DF 0x4000			/* dont fragment flag */
// #define	IP_MF 0x2000			/* more fragments flag */
// #define	IP_OFFMASK 0x1fff		/* mask for fragmenting bits */
//     u_int8_t ip_ttl;			/* time to live */
//     u_int8_t ip_p;			/* protocol */
//     u_short ip_sum;			/* checksum */
//     struct in_addr ip_src, ip_dst;	/* source and dest address */
//   };


  // struct in_addr {
  //      in_addr_t s_addr;
  //  };

 // in_addr_t Equivalent to the type uint32_t as described in <inttypes.h>

//  The function inet_ntoa() converts a network address in a struct in_addr to a dots-and-numbers format string. 
// The "n" in "ntoa" stands for network, and the "a" stands for ASCII for historical reasons 
// (so it's "Network To ASCII"—the "toa" suffix has an analogous friend in the C library called atoi() which converts an ASCII string to an integer.)

// The function inet_aton() is the opposite, converting from a dots-and-numbers string into a in_addr_t 
// (which is the type of the field s_addr in your struct in_addr.)

