/*
 * NetlinkConn.h
 *
 *  Created on: Aug 13, 2017
 *      Author: root
 */

#ifndef NETLINKCONN_H_
#define NETLINKCONN_H_
#define NETLINK_PACKET_INFO 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/

#include <sys/socket.h>
#include <linux/netlink.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

class NetlinkConn {
public:
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh;
	struct iovec iov;
	int sock_fd;
	struct msghdr msg;
	NetlinkConn();
	~NetlinkConn();
	long int recvmsg();
	unsigned char* getMsg();
};

#endif /* NETLINKCONN_H_ */
