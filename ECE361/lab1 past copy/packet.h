// packet.h
// author: JMP

#pragma once

#define DPKT_SZ sizeof(datapkt_t)
typedef struct
{
    uint64_t total_frag;
    uint64_t frag_no;
    uint64_t size;
    char* filename;
    uint8_t filedata[1000];
} __attribute__((aligned(8), packed)) datapkt_t;

#define APKT_SZ sizeof(ackpkt_t)
typedef struct
{
    uint64_t crc64;
} __attribute__((aligned(8), packed)) ackpkt_t;
