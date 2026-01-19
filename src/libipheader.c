#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/udp.h>
#include <ctype.h>
#include <string.h>
#define MAX_IP_HLIMIT 65536
typedef struct ether_header ethernetframe_t;
typedef struct iphdr ipheaderv4_t;
typedef struct tcphdr tcpheaderv4_t;
typedef struct udphdr udpheader_t;
void parse_http(const char *payload, int payload_size){
/* A Standard HTTP Shit:
GET / HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n\r\n
<html>
header_end points to the \r\n\r\n shit. header_end + 4 points to the fucking body.*/
char *header_end = strstr(payload, "\r\n\r\n");
if (header_end != NULL){
size_t header_length = header_end - payload;
printf("\nHTTP Headers:\n");
printf("%.*s",(int)header_length, payload);
char *body = header_end + 4;
printf("%s", body);
}
}


void print_packet_headerinfo(const char *buffer, int packet_size){
int i;

/* Ethernet Header Info */
ethernetframe_t *efr = (ethernetframe_t *)buffer;
u_int8_t *dest_mac = efr->ether_dhost;/*ether_dhost is also a pointer to a u_int8_t type which stores the 6 bytes of the destination mac.*/
u_int8_t *smac = efr->ether_shost;/* ether_shost is actually a pointer to a u_int8_t type which stores the 6 bytes of the source mac.*/
u_int16_t eth_type = ntohs(efr->ether_type); /* ether_type is 16bits. must use u_int16_t to avoid truncation of the received ether_type. since the received ether_type will be in network byte order, we can convert it to human readable string with ntohs().*/


ipheaderv4_t* iph = (ipheaderv4_t *)(buffer + 14);

/* IP HEADER INFO: */
uint8_t iph_length = iph->ihl * 4; /* got from google. ihl is in 32 bit words, multiply by 4 to get in bytes. */
uint8_t protocol = iph->protocol; /* 6 = TCP, 17 = UDP */
uint32_t srcaddr = iph->saddr; /* in network byte order */
uint32_t dstaddr = iph->daddr; /* in network byte order */
char srcaddr_str[INET_ADDRSTRLEN];
char dstaddr_str[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &srcaddr, srcaddr_str, sizeof(srcaddr_str));
inet_ntop(AF_INET, &dstaddr, dstaddr_str, sizeof(dstaddr_str));

/* TCP HEADER INFO: */
if (iph_length >= packet_size){
printf("\nInvalid IP Header Length of the packet.");
return;
}
if (iph_length + sizeof(tcpheaderv4_t) > packet_size){
printf("\nPacket too small for TCP+IP Headers.");
return; /* doesn't make sense because i will intentionally create a malformed packet later and make sure this sucker isn't sleeping ;) */
}
if (iph_length + sizeof(udpheader_t) > packet_size){
printf("\nMalformed UDP Packet.");
return;
}
if (protocol == IPPROTO_TCP){
tcpheaderv4_t *tcph = (tcpheaderv4_t *)(buffer + iph_length + 14);
/* A buffer array is occupied by bytes logic in a packet. After n bytes, TCP Header starts. */
uint16_t tcph_length = tcph->doff * 4;
char *payload = (char *)(tcph + tcph_length);
int payload_length = packet_size - 14 - iph_length - tcph_length;
uint16_t srcport = ntohs(tcph->source); /* in network byte order */
uint16_t destport = ntohs(tcph->dest); /* in network byte order */
uint32_t syn = ntohl(tcph->seq); /* in network byte order */
uint32_t ack = ntohl(tcph->ack_seq); /* in network byte order */
char pr[10] = "TCP";
printf("\n%s.%d > %s.%d [Protocol: %s/%d, IHL: %d, SEQ: %u, ACK: %u]", srcaddr_str, srcport, dstaddr_str, destport, pr, protocol, iph_length, syn, ack);
printf("\n[ETHERNET FRAME]: DMAC: ");
for (i=0; i<6; i++){ /* mac = 6 bytes.*/
if (i > 0 && i <= 5){
printf(":%02x", dest_mac[i]);
}
else if (i == 0 || i == 5){
printf("%02x", dest_mac[i]);
}
}
printf(" SMAC: ");
for (i=0; i<6; i++){ /* mac = 6 bytes.*/
if (i > 0 && i <= 5){
printf(":%02x", smac[i]);
}
else if (i == 0 || i == 5){
printf("%02x", smac[i]);
}
}
printf("  Protocol Type: %u", eth_type);
printf("\nTCP Payload (%d Bytes): ", payload_length);
printf("\n");
for(i=0; i < payload_length; i++){
if (isprint(payload[i])){
printf("%c", payload[i]);
}
else {
printf(".");
}
}
}
else if (protocol == IPPROTO_UDP){
udpheader_t *udp = (udpheader_t *)(buffer + iph_length + 14);
char pr[10] = "UDP";
u_int16_t source_port = ntohs(udp->uh_sport);
u_int16_t dest_port = ntohs(udp->uh_dport);
u_int16_t plen = ntohs(udp->uh_ulen);
u_int16_t csum = ntohs(udp->uh_sum);

printf("\n%s.%d > %s.%d [Protocol: %s/%d, IHL: %d, Length: %d, Checksum: 0x%04x]", srcaddr_str, source_port, dstaddr_str, dest_port, pr, protocol, iph_length, plen, csum);
printf("\n[ETHERNET FRAME]: DMAC: ");
for (i=0; i<6; i++){ /* mac = 6 bytes.*/
if (i > 0 && i <= 5){
printf(":%02x", dest_mac[i]);
}
else if (i == 0 || i == 5){
printf("%02x", dest_mac[i]);
}
}
printf(" SMAC: ");
for (i=0; i<6; i++){ /* mac = 6 bytes.*/
if (i > 0 && i <= 5){
printf(":%02x", smac[i]);
}
else if (i == 0 || i == 5){
printf("%02x", smac[i]);
}
}
}


else {
printf("\nNot a TCP/UDP Packet.");
}
}


