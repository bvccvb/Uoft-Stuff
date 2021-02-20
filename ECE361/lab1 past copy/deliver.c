// server.c
// author: JMP

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void send_to_server(int socket, struct sockaddr_in server){
{
	size_t len = sizeof(server);

//typedef struct
//{
//    uint64_t total_frag;
    uint64_t frag_no;
    uint64_t size;
    char* filename;
    uint8_t filedata[1000];
} __attribute__((aligned(8), packed)) datapkt_t;
	sendto(socket, "GOODBYE WORLD", 13, 0, (struct sockaddr*)&server, len);
	return;
}

int
main(int argc, char** argv)
{
    uint32_t skfd;
	struct hostent* hp;
	struct sockaddr_in server;

	//This gets the host
	if ((hp = gethostbyname("ug205.eecg.utoronto.ca")) == 0) {
        printf("Invalid or unknown host\n");
        exit(1);
    }

	//Initializes the server socket
	memcpy(&server.sin_addr.s_addr, hp->h_addr, hp->h_length);

	//socket is created
    if ((skfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[!] socket() failed");
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(5000);

	//Casting server into stuck sockaddr*
    //if (bind(skfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
    //    perror("[!] bind() failed");
    //    exit(-1);
    //}

	send_to_server(skfd, server);

    return 0;
}
