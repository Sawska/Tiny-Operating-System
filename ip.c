#include "ip.h"
#include "uart.h"
#include <stdint.h>
#include <stddef.h>
#include "slip.h"

static uint8_t local_ip[IP_ADDR_LEN];

static uint16_t ip_checksum(void* vdata, size_t length) {
    uint8_t* data = (uint8_t*)vdata;
    uint32_t sum = 0;
    for (size_t i = 0; i < length; i += 2) {
        uint16_t word = data[i] << 8;
        if (i + 1 < length)
            word |= data[i + 1];
        sum += word;
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return ~sum;
}


static void my_memcpy(uint8_t* dest, const uint8_t* src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}


static int my_memcmp(const uint8_t* a, const uint8_t* b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return (int)(a[i]) - (int)(b[i]);
        }
    }
    return 0;
}

void ip_init(uint8_t* ip_addr) {
    my_memcpy(local_ip, ip_addr, IP_ADDR_LEN);
}

void ip_send_icmp_echo_reply(const uint8_t* request_packet, uint16_t len) {
    const ip_header_t* req_ip = (const ip_header_t*)request_packet;
    uint16_t ip_header_len = (req_ip->version_ihl & 0x0F) * 4;

    uint16_t total_len = (req_ip->total_length >> 8) | ((req_ip->total_length & 0xFF) << 8); // endian fix
    if (len < total_len) return;

    static uint8_t reply_buf[1500];
    my_memcpy(reply_buf, request_packet, total_len);

    ip_header_t* rep_ip = (ip_header_t*)reply_buf;

    my_memcpy(rep_ip->src, req_ip->dst, IP_ADDR_LEN);
    my_memcpy(rep_ip->dst, req_ip->src, IP_ADDR_LEN);

    rep_ip->ttl = 64;
    rep_ip->header_checksum = 0;
    rep_ip->header_checksum = ip_checksum(rep_ip, ip_header_len);

    uint8_t* icmp = reply_buf + ip_header_len;
    icmp[0] = 0;  

    icmp[2] = 0;
    icmp[3] = 0;
    uint16_t icmp_len = total_len - ip_header_len;
    uint16_t checksum = ip_checksum(icmp, icmp_len);
    icmp[2] = (checksum >> 8) & 0xFF;
    icmp[3] = checksum & 0xFF;

    slip_send(reply_buf, total_len);

    uart_puts("ICMP Echo Reply sent via SLIP\r\n");
}

void ip_input(const uint8_t* data, uint16_t len) {
    if (len < sizeof(ip_header_t)) return;

    const ip_header_t* ip_hdr = (const ip_header_t*)data;

    if ((ip_hdr->version_ihl >> 4) != 4) return;

    uint16_t header_len = (ip_hdr->version_ihl & 0x0F) * 4;
    if (header_len < 20) return;

    if (my_memcmp(ip_hdr->dst, local_ip, IP_ADDR_LEN) != 0) {
        return;
    }

    uint16_t recv_checksum = ip_hdr->header_checksum;
    ((ip_header_t*)ip_hdr)->header_checksum = 0;
    uint16_t calc_checksum = ip_checksum((void*)ip_hdr, header_len);
    if (recv_checksum != calc_checksum) return;

    switch(ip_hdr->protocol) {
        case 1:  
            if (len < header_len + 8) return;
            if (data[header_len] == 8) { 
                ip_send_icmp_echo_reply(data, len);
            }
            break;

        case 6:  
        case 17: 
            uart_puts("Received TCP/UDP packet (not implemented)\r\n");
            break;

        default:
            uart_puts("Unknown IP protocol\r\n");
    }
}
