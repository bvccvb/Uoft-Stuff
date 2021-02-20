// server.c
// author: JMP

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "crc64.h"
#include "map.h"
#include "packet.h"

void
write_file(const char* filename, uint8_t* data, size_t nbytes)
{
    FILE* write_ptr = fopen(filename, "wb");
    fwrite(data, nbytes, 1, write_ptr);
}

void
receive_file(uint32_t sd)
{
    typedef struct
    {
        size_t nbytes;
        uint8_t* data;
    } file_t;

    map_void_t m;
    map_init(&m);

    FILE* write_ptr = fopen("cat.jpeg", "wb");

    while (1) {
        printf("Recieving data packets\n");

        uint8_t buf[DPKT_SZ];
        unsigned char* filename;
        struct sockaddr_in skin;
        uint32_t len = sizeof(skin);

        // receive a packet
        if (recvfrom(sd, buf, DPKT_SZ, 0, (struct sockaddr*)&skin, &len) < 0) {
            perror("[!] recvfrom() failed");
            exit(-1);
        }

        printf("%d - %d - %d - %d - %d - %d - %d - %d\n", buf[8], buf[333],buf[653],buf[798],buf[647], buf[900], buf[1023], buf[DPKT_SZ]);

        // send back CRC as acknowledgement
        uint64_t crc = crc64(0, buf, DPKT_SZ);
        ackpkt_t ack = { .crc64 = crc };
        printf("Sending acknowledgement\n\n");
        sendto(sd, &ack, APKT_SZ, 0, (struct sockaddr*)&skin, len);

        // deserialize packet and add it to file table

        datapkt_t pkt;
        //printf("%d\n", pkt);
        memcpy(&pkt, buf, DPKT_SZ);

        printf("Total frag is %d\n", pkt.total_frag);
        printf("Fragment Number is %d\n", pkt.frag_no);
        printf("size is %d\n", pkt.size);
        //filename = (unsigned char *) pkt.filename;
        printf("File name is %s.\n", &pkt.filename);
        printf("File data is %d - %d.\n\n", pkt.filedata[0], pkt.filedata[DPKT_SZ]);

        fwrite(pkt.filedata, 1000, 1, write_ptr);   

        if(pkt.total_frag == pkt.frag_no){
            fclose (write_ptr); 
            break;
        }   

        }
        printf("File Transfer Completed\n");

}

int main (int argc, char **argv) {

    //printf("%s", "Hello world");

    uint32_t sock_fd, new_sock_fd;
    int port_number;
    char client_message[100] = {0};
    struct sockaddr_in server_address, address_2;

    //get port number
    port_number = atoi(argv[1]);
    
    //set addressing scheme to AF_INET (IP) 
    server_address.sin_family = AF_INET;
    //listen on port 3490
    server_address.sin_port = htons(port_number);
    //allow any IP to connect
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    
    sock_fd = socket (PF_INET, SOCK_DGRAM, 0);

    //prepare to listen from port specified in sockaddr (sever_address)
    bind(sock_fd, (struct sockaddr *) &server_address, sizeof(server_address));

    //while (1){
        printf("Recieving\n");
        uint32_t addr_size = sizeof(address_2);
        bzero(client_message, 100);
        recvfrom(sock_fd, client_message, 100, 0, (struct sockaddr *) &address_2, &addr_size);

        printf("Recieved message: %s\n", client_message);

        if(strcmp(client_message, "ftp") == 0){
            sendto(sock_fd, "yes", 100, 0, (struct sockaddr *) &address_2, addr_size);
            receive_file(sock_fd);
        } else {
            sendto(sock_fd, "no", 100, 0, (struct sockaddr *) &address_2, addr_size);
        }

    //}
    
    printf("Closing connection.\n");
    close (sock_fd);

    return 0;
}
