#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdbool.h>
#include "packet_parser.h"
#include <net/ethernet.h>
#include <net/if.h> // for if_nametoindex
#include <linux/if_packet.h> // for sockaddr_ll, net/ethernet.h doesnt fix this shit
volatile sig_atomic_t sigint_flag; /* @brief: global variable.*/
void sigint_handler(int captured_sig_repr){
if (captured_sig_repr == SIGINT){
sigint_flag = 1;
}
}
int main(){
char interface_name[IFNAMSIZ];
int socket_descriptor = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));
char receive_buffer[65536]; /* @brief: MAX size of IP Packet: 65535 bytes + \0*/
/* @brief for volatile sig_atomic_t sigint_flag:  This is no Nuclear Launch
code shit. 'volatile' tells the compiler that the variable is more likely to
be changed so don't fuck with it. sig_atomic_t is not a data type to store
nuclear launch codes, it allows us to run an atomic cpu function: DO IT ALL OR
NEVER DO IT instruction to the CPU. This makes us less prone to other unprecedented
interruptions.*/

/* @brief: we are only capturing a limited number of
packets for testing.*/
ssize_t packet_size; /* @brief: signed integer type, allows us to pinpoint errors. */
/* @thorough:
Specifying AF_INET creates an IPv4 socket. The kernel strips off the Ethernet
Header fields of the packet and filters them out since we specified an IPv4
socket. By specifying IPv4, we work at the Layer 3: IP Layer of the OSI Model.

Specifying SOCK_RAW allows us to bypass the Kernel processing of TCP/UDP headers
and allows us to interact with packets through raw sockets. This allows us to
create custom crafted packets from Layer 3, and also capture packets.
We can capture packets because when a raw socket is not bound to an address,
it bounds itself to a port on the specified interface.
Then, it receives packets passing through that interface, and can send packets
too.
Kernel Demultiplexing is when a kernel decides which packet to send to which raw
socket through the specification of a certain macro in the socket() function's
third parameter which specifies the Transport Layer protocol to be used, like
IPPROTO_TCP.*/
signal(SIGINT, sigint_handler);
if (socket_descriptor < 0){
perror("socket");
printf("[!] Socket Creation Failed.\n");
return 1;
}
printf("[!] Socket Created at File Descriptor: %d\n", socket_descriptor);
struct sockaddr_ll mybeautifulsocket;

/* @debug: intended to bind to wlan0. forgot that LMAO. my initial ip was getting overwritten generously. this is the struct where received packet details are stored.*/
struct sockaddr_ll bindaddr = {0};
/* @debug: This is the struct that stores the relevant info to bind the socket at wlan0 interface with its IP.*/
bindaddr.sll_ifindex = if_nametoindex("wlan0");
bindaddr.sll_family = AF_PACKET;
bindaddr.sll_protocol = htons(ETH_P_IP);

if ((bind(socket_descriptor, (struct sockaddr *)&bindaddr, sizeof(bindaddr))) < 0) {
perror("bind");
return 1;
}
/* @brief:
inet_ntop() -> Convert IP from Numeric to Human-readable string.
inet_ntoa() -> Convert IP from Binary to Human-Readable string:
char *inet_ntoa(struct in_addr in);
*/
if_indextoname(bindaddr.sll_ifindex, interface_name);
printf("Socket Bind at: %s\n\n", interface_name);
socklen_t socklength = sizeof(mybeautifulsocket);
/* @debugged: usr/include/x86_64-linux-gnu/sys/socket.h:165:48: note: expected ‘socklen_t * restrict’ {aka ‘unsigned int * restrict’} but argument is of type ‘struct sockaddr_in *’
                          socklen_t *__restrict __addr_len);
*/

while (!sigint_flag){
packet_size = recvfrom(socket_descriptor, receive_buffer, sizeof(receive_buffer), 0, (struct sockaddr *)&mybeautifulsocket, &socklength);
/* ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen); */
if (packet_size < 0){
perror("recvfrom");
break;
}


/* const char *inet_ntop(int af, const void *restrict src,
                      char *restrict dst, socklen_t size); */

printf("Packet Captured: Size in Bytes: %zd", packet_size);
print_packet_headerinfo(receive_buffer, packet_size);
printf("\n");
printf("\n");
sleep(1);
}

printf("\nCaught CTRL + C. Closing the Socket and Exiting the Program...");
close(socket_descriptor);
return 0;
}
