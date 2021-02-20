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

    while (1) {
        printf("Recieving data packets\n");

        datapkt_t data;
        uint8_t buf[DPKT_SZ];
        struct sockaddr_in skin;
        uint32_t len = sizeof(skin);

        // receive a packet
        if (recvfrom(sd, &data, DPKT_SZ, 0, (struct sockaddr*)&skin, &len) < 0) {
            perror("[!] recvfrom() failed");
            exit(-1);
        }

        // send back CRC as acknowledgement
        uint64_t crc = crc64(0, data.filedata, DPKT_SZ);
        ackpkt_t ack = { .crc64 = crc };
        printf("Sending acknowledgement\n");
        sendto(sd, &ack, APKT_SZ, 0, (struct sockaddr*)&skin, len);

        // deserialize packet and add it to file table
        datapkt_t pkt;
        memcpy(&pkt, data.filedata, DPKT_SZ);

        file_t* file = NULL;
        void* mentry = map_get(&m, pkt.filename);

        // register file in the file table
        if (mentry == NULL) {
            file = calloc(1, sizeof(file_t));
            file->nbytes = 0;
            file->data = calloc(pkt.total_frag, sizeof(pkt.filedata));
            map_set(&m, pkt.filename, (void*)file);
        } else {
            file = *((file_t**)mentry);
        }

        assert(file);
        assert(file->data);

        // transfer completed
        if (pkt.size == 0) {
            // write to disk
            write_file(pkt.filename, file->data, file->nbytes);

            // remove in-memory buffer
            free(file->data);
            free(file);

            // deallocate from file table
            map_remove(&m, pkt.filename);

            continue;
        }

        // copy the file data to file table
        memcpy(&file->data[pkt.frag_no * sizeof(pkt.filedata)],
               pkt.filedata,
               pkt.size);

        // naive way to handle file size (assume 1...n-1 packets are full)
        file->nbytes = (pkt.total_frag - 1) * sizeof(pkt.filedata) + pkt.size;
    }
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
