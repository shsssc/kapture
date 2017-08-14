/*
 * NetlinkConn.cpp
 *
 *  Created on: Aug 13, 2017
 *      Author: root
 */

#include "NetlinkConn.h"

NetlinkConn::NetlinkConn() {

	sock_fd = socket( PF_NETLINK, SOCK_RAW, NETLINK_PACKET_INFO);
	if (sock_fd < 0)
		return;

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid(); /* self pid */

	bind(sock_fd, (struct sockaddr *) &src_addr, sizeof(src_addr));

	memset(&dest_addr, 0, sizeof(dest_addr));
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; /* For Linux Kernel */
	dest_addr.nl_groups = 0; /* unicast */

	nlh = (struct nlmsghdr *) malloc(NLMSG_SPACE(MAX_PAYLOAD));
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_flags = 0;

	strcpy((char*) NLMSG_DATA(nlh), "Hello");
	iov.iov_base = (void *) nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *) &dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	printf("Sending message to kernel\n");
	sendmsg(sock_fd, &msg, 0);
	printf("Waiting for message from kernel\n");

	/* Read message from kernel */
	::recvmsg(sock_fd, &msg, 0);
	printf("Received message payload: %s\n", (char *) NLMSG_DATA(nlh));

}

NetlinkConn::~NetlinkConn() {
	// TODO Auto-generated destructor stub
	close(sock_fd);
}

long int NetlinkConn::recvMsg() {
	return ::recvmsg(sock_fd, &msg, 0);
}

unsigned char* NetlinkConn::getMsg() {
	return (unsigned char *) NLMSG_DATA(nlh);
}

