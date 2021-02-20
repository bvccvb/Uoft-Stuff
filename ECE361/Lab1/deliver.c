// server.cdeliver
// author: JMP, LFN

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>


#include "crc64.h"
#include "packet.h"

#define BUF_S (1 << 6)
#define BUF_M (1 << 8)
#define BUF_L (1 << 10)

uint64_t
get_us_since_epoch()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_usec);
}

void
get_ip_from_hostname(char** argv, char* IP)
{
    struct hostent* host_info;
    struct in_addr** address_list;

    if ((host_info = gethostbyname(argv[1])) == NULL) {
        herror("gethostbyname");
        return;
    }
    address_list = (struct in_addr**)host_info->h_addr_list;

    for (int i = 0; address_list[i] != NULL; i++) {
        strcpy(IP, inet_ntoa(*address_list[i]));
    }
    return;
}

int
main(int argc, char** argv)
{
    // printf ("hi");
    int sock_fd, n, port_number;
    char ip_address[100];

    char send_message[BUF_S];
    char server_ack[BUF_S];
    char server_name;
    struct sockaddr_in server_address, address_2;
    struct addrinfo temp, *server_info;

    double estimated_RTT;
    double sample_RTT;
    double deviation_RTT;
    double timeout_value;

    uint64_t now1;
    uint64_t after1;

    FILE* fp;

    // setting up socket
    sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
    bzero(&server_address, sizeof server_address);

    // get port and host number from arguments
    get_ip_from_hostname(argv, ip_address);
    printf("IP adress is %s\n", ip_address);
    port_number = atoi(argv[2]);
    printf("Port number is %d\n", port_number);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);

    inet_pton(AF_INET, ip_address, &(server_address.sin_addr));

    //Setting up the timeout;
    struct timeval timeout;
    timeout.tv_sec = 1;

    bzero(send_message, BUF_S);
    bzero(server_ack, BUF_S);

    printf("ftp ");
    scanf("%s", &send_message);
    uint32_t addr_size = sizeof(server_address);
    uint32_t addr_size2 = sizeof(address_2);

    // check if file exist
    if (fp = fopen(send_message, "r")) {
        uint64_t now;
        uint64_t after;
        now = get_us_since_epoch();
        sendto(sock_fd,
               "ftp",
               BUF_S,
               0,
               (struct sockaddr*)&server_address,
               addr_size);
        recvfrom(sock_fd,
                 server_ack,
                 BUF_S,
                 0,
                 (struct sockaddr*)&address_2,
                 &addr_size2);
        after = get_us_since_epoch();
        estimated_RTT = after - now;
        printf("Initial Connection took %d microseconds\n", after - now);

        printf("%s\n", server_ack);
    } else {
        sendto(sock_fd,
               "File not sent",
               BUF_S,
               0,
               (struct sockaddr*)&server_address,
               addr_size);
        printf("Closing connection.\n");
        close(sock_fd);
        return 0;
    }

    if (strcmp(server_ack, "yes") == 0) {

        printf("A file transfer can start.\n");

        int file_size;
        uint64_t crc;
        bool transfer_success;
        datapkt_t sending;
        uint8_t buf[DPKT_SZ];
        ackpkt_t ack;

        struct stat file_stats;
        strncpy(sending.filename, send_message, sizeof(sending.filename));
        printf("File name is %s.\n", sending.filename);

        stat(sending.filename, &file_stats);
        file_size = file_stats.st_size;
        printf("File size is %d bytes.\n", file_size);

        sending.total_frag = (file_size / 1000) + 1;
        printf("Total frag is %d\n\n", sending.total_frag);

        sending.frag_no = 1;
        sending.size = 0;
        transfer_success = true;

        now1 = get_us_since_epoch();

        while (sending.frag_no < sending.total_frag + 1) {

            setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

            // Get file ready to send
            if (transfer_success == true) {
                bzero(buf, DPKT_SZ);
                bzero(&sending.filedata, 1000);

                fread(sending.filedata, 1000, 1, fp);

                if (sending.frag_no < sending.total_frag) {
                    sending.size = 1000;
                } else if (sending.frag_no == sending.total_frag + 1) {
                    sending.size = 0;
                } else {
                    sending.size = file_size % 1000;
                }
            }

            transfer_success = false;

            // serialize
            memcpy(buf, &sending, DPKT_SZ);
            crc = crc64(0, buf, DPKT_SZ);
            printf("sending is %x\n\n", crc);

            // Sending filedata
            now1 = after1;
            sendto(sock_fd,
                   buf,
                   DPKT_SZ,
                   0,
                   (struct sockaddr*)&server_address,
                   addr_size);

            // Receiving Acknowledge
            recvfrom(sock_fd,
                     &ack,
                     APKT_SZ,
                     0,
                     (struct sockaddr*)&address_2,
                     &addr_size2);
            after1 = get_us_since_epoch();


            sample_RTT = after1 - now1;

            estimated_RTT = 0.875 * estimated_RTT + 0.125 * sample_RTT;
            deviation_RTT = 0.75 * deviation_RTT + 0.25 * abs(sample_RTT - estimated_RTT);
            timeout_value = estimated_RTT + 4 * deviation_RTT;


            timeout.tv_sec = 0;
            timeout.tv_usec = timeout_value + 50000;


            printf("Received %x\n", ack.crc64);
            printf("Original %x\n\n", crc);

            if (ack.crc64 == crc) {
                printf("Acknowledge Received\n");
                transfer_success = true;
            } else {
                printf("Acknowledge error, retransmitting.\n\n");
                transfer_success = false;
            }

            if (transfer_success)
                sending.frag_no++;
        }
        printf("File Transfer Completed\n");
        printf("Closing connection.\n");
        close(sock_fd);
        return 0;
    } else {
        printf("Closing connection.\n");
        close(sock_fd);
        return 0;
    }
}
