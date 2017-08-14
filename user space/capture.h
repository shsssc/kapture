/*
 * capture.h
 *
 *  Created on: Aug 11, 2017
 *      Author: root
 */

#ifndef CAPTURE_H_
#define CAPTURE_H_

#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>

#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]

#define NETLINK_PACKET_INFO 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/

#define PACKET_BUFFER_SIZE 5000000
#define PACKET_BUFFER_NUM  3

struct pktinfo {
	__be32 sip, dip;
	unsigned int seq;
	unsigned short sport, dport, pkt_len;
};

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;

pktinfo* packet_buffers[PACKET_BUFFER_NUM];
unsigned char current_buffer;
unsigned int packet_buffer_pointer[PACKET_BUFFER_NUM];

void store_package(unsigned char* pkt);
int init_connection();
void* storage_loop(void*);
void* buffer_changer(void*);

#endif /* CAPTURE_H_ */
