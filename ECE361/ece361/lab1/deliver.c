#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

#include <sys/stat.h>
#include "crc64.h"
#include "packet.h"

void get_IP_from_hostname (char **argv, char * IP){

	struct hostent *host_info;
	struct in_addr **address_list;

	if ((host_info = gethostbyname(argv[1])) == NULL) { 
		herror("gethostbyname");
		return;
	}
	address_list = (struct in_addr **)host_info->h_addr_list;

	for(int i = 0; address_list[i] != NULL; i++) {
		strcpy(IP, inet_ntoa(*address_list[i]));
	}
	return;
}

int main (int argc, char **argv) {
	//printf ("hi");
	int sock_fd, n, port_number;
	char* IP_address;

	char send_message[100];
	char server_aknowledge[100];
	char server_name;
	struct sockaddr_in server_address, address_2;
	struct addrinfo temp, *server_info;

	FILE *fp;

	//setting up socket
	sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
	bzero(&server_address, sizeof server_address);
	
	//get port and host number from arguments
	get_IP_from_hostname(argv, IP_address); 
	printf ("IP adress is %s\n", IP_address);
	port_number = atoi(argv[2]);	
	printf ("Port number is %d\n", port_number);

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port_number);

	//sends to itself 127.0.0.1 is a loop
	//inet_pton(AF_INET, "127.0.0.1", &(server_address.sin_addr));
	inet_pton(AF_INET, IP_address, &(server_address.sin_addr));
	
	//connecting to server
	//connect(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address));

	//while(1){
	//bzero(send_message, 100);
	//bzero(server_aknowledge, 100);

	printf("ftp ");
	scanf("%s", &send_message);
	//printf("\n");
	uint32_t addr_size = sizeof(server_address);
	uint32_t addr_size2 = sizeof(address_2);
	
	//check if file exist
	if(fp = fopen(send_message, "r")){
		sendto(sock_fd, "ftp", 100, 0, (struct sockaddr *) &server_address, addr_size);
		recvfrom(sock_fd, server_aknowledge, 100, 0, (struct sockaddr *) &address_2, &addr_size2);
		printf("%s\n", server_aknowledge);
	} else {
		sendto(sock_fd, "File not sent", 100, 0, (struct sockaddr *) &server_address, addr_size);
		printf("Closing connection.\n");
		close (sock_fd);
		return 0;
	}

	if(strcmp(server_aknowledge, "yes") == 0){
		printf("A file transfer can start.\n");

		//fp is holding the pointer to the file
		datapkt_t sending;
		struct stat file_stats;
		sending.filename = send_message;
		printf("File name is %s.\n", sending.filename);
		stat(sending.filename, &file_stats);
		sending.size = file_stats.st_size;
		printf("File size is %d bytes.\n", sending.size);
		sending.total_frag = (sending.size/1000) + 2;
		printf("Total frag is %d\n", sending.total_frag);

		uint64_t crc;
		bool transfer_success;
		ackpkt_t ack;
		sending.frag_no = 0;

		while(1){

			transfer_success = false;

			//Get file ready to send
			fread (sending.filedata, 1000, 1, fp);
			crc = crc64(0, sending.filedata, DPKT_SZ);
			ack = { .crc64 = crc };

			//Sending filedata
			sendto(sock_fd, sending.filedata, DPKT_SZ, 0, (struct sockaddr *) &server_address, addr_size);

			//Recieving Acknowledge
			recvfrom(sock_fd, server_aknowledge, 100, 0, (struct sockaddr *) &address_2, &addr_size2);

			if(){

			}

			//Check if the 
			sending.frag_no ++;
		}

		printf("Closing connection.\n");
		close (sock_fd);
		return 0;
	} else {
		printf("Closing connection.\n");
		close (sock_fd);
		return 0;
	}

	//}

}

