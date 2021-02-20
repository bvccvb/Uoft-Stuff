// server.c
// author: JMP, LFN

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "crc64.h"
#include "map.h"
#include "packet.h"

#define BUF_S (1 << 6)
#define BUF_M (1 << 8)
#define BUF_L (1 << 10)

uint64_t
get_ms_since_epoch()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

void
receive_file(uint32_t sd)
{
    char filename[100] = { 0 };
    sprintf(filename, "%llu.data", get_ms_since_epoch);
    FILE* fptr = fopen(filename, "wb");

    while (1) {
        puts("[*] Receiving data packets...");

        uint8_t buf[DPKT_SZ];
        struct sockaddr_in skin;
        uint32_t len = sizeof(skin);

        // receive a packet
        if (recvfrom(sd, buf, DPKT_SZ, 0, (struct sockaddr*)&skin, &len) < 0) {
            perror("[!] recvfrom() failed");
            exit(-1);
        }

        // send back CRC as acknowledgement
        {
            uint64_t crc = crc64(0, buf, DPKT_SZ);
            ackpkt_t ack = { .crc64 = crc };
            puts("[*] Sending acknowledgement...");
            sendto(sd, &ack, APKT_SZ, 0, (struct sockaddr*)&skin, len);
        }

        // deserialize packet
        datapkt_t pkt;
        memcpy(&pkt, buf, DPKT_SZ);

        printf("[*] total_frag: %d\n", pkt.total_frag);
        printf("[*] frag_no: %d\n", pkt.frag_no);
        printf("[*] size: %d\n", pkt.size);
        printf("[*] filename: %s\n", pkt.filename);

        // write to file
        fwrite(pkt.filedata, pkt.size, 1, fptr);

        if (pkt.total_frag == pkt.frag_no) {
            fclose(fptr);
            rename(filename, pkt.filename);
            break;
        }
    }

    puts("[!] File transfer completed");
}

void
handle_ftp_conn(uint32_t skfd,
                char* req_msg,
                struct sockaddr* addr,
                socklen_t addr_sz)
{
    if (strcmp(req_msg, "ftp") != 0) {
        sendto(skfd, "no", BUF_S, 0, addr, addr_sz);
        return;
    }

    sendto(skfd, "yes", BUF_S, 0, addr, addr_sz);
    receive_file(skfd);
}

int
main(int argc, char** argv)
{
    uint32_t skfd;
    uint16_t port;
    struct sockaddr_in serv_addr, conn_addr;
    struct sockaddr *serv_addr_sa = (struct sockaddr*)&serv_addr,
                    *conn_addr_sa = (struct sockaddr*)&conn_addr;

    if (argc != 2 || (port = strtoul(argv[1], NULL, 10)) == 0) {
        printf("[!] Usage: %s <UDP listen port>\n", argv[0]);
        exit(-1);
    }

    // get port number
    port = atoi(argv[1]);

    // set addressing scheme to AF_INET (IP)
    serv_addr.sin_family = AF_INET;

    // listen on port
    serv_addr.sin_port = htons(port);

    // allow any IP to connect
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);

    // create a server socket
    if ((skfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("[!] socket() failed");
        exit(-1);
    }

    // prepare to listen from port specified in sockaddr (sever_address)
    if (bind(skfd, serv_addr_sa, sizeof(serv_addr)) == -1) {
        perror("[!] bind() failed");
        exit(-1);
    }

    while (1) {
        puts("[*] Receiving...");

        char req_msg[BUF_S] = { 0 };
        socklen_t addr_sz = sizeof(conn_addr);

        // receive message from client, and store connection info in conn_addr
        recvfrom(skfd, req_msg, sizeof(req_msg), 0, conn_addr_sa, &addr_sz);

#if 0
        // fork and skip parent process
        if (fork() != 0)
            continue;
#endif

        printf("[*] Received message: %s\n", req_msg);

        // handle connection as ftp
        handle_ftp_conn(skfd, req_msg, conn_addr_sa, addr_sz);

#if 0
        break;
#endif
    }

    puts("[*] Closing connection.");
    close(skfd);

    return 0;
}
