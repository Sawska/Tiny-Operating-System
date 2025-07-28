#ifndef IP_H
#define IP_H

#include <stdint.h>

#define IP_ADDR_LEN 4

typedef struct {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint8_t src[IP_ADDR_LEN];
    uint8_t dst[IP_ADDR_LEN];
} __attribute__((packed)) ip_header_t;

void ip_init(uint8_t* ip_addr);
void ip_input(const uint8_t* data, uint16_t len);
void ip_send_icmp_echo_reply(const uint8_t* request_packet, uint16_t len);

#endif
