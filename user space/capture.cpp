
#include "capture.h"
void store_package(unsigned char* pkt) {

	unsigned char tmp_current_buffer = current_buffer;
	tmp_current_buffer = (tmp_current_buffer % PACKET_BUFFER_NUM); //dealing with a racing condition problem
	unsigned int& tmp_current_buffer_pointer =
			packet_buffer_pointer[tmp_current_buffer];

	pktinfo tmpPkt;
	tmpPkt.sip = *((__be32 *) (pkt));
	tmpPkt.dip = *((__be32 *) (pkt + 4));
	tmpPkt.sport = *((unsigned short *) (pkt + 8));
	tmpPkt.dport = *((unsigned short *) (pkt + 10));
	tmpPkt.pkt_len = *((unsigned short *) (pkt + 12));
	tmpPkt.seq = *((unsigned int *) (pkt + 14));

	packet_buffers[tmp_current_buffer][tmp_current_buffer_pointer++] = tmpPkt;

	/*printf("writing to buffer %u index %u\n", tmp_current_buffer,
			tmp_current_buffer_pointer - 1);*/
	printf(
	 "Packet for source address: %u.%u.%u.%u:%u destination address: %u.%u.%u.%u:%u len: %u seq: %u\n",
	 NIPQUAD(tmpPkt.sip), tmpPkt.sport, NIPQUAD(tmpPkt.dip),
	 tmpPkt.dport, tmpPkt.pkt_len, tmpPkt.seq);

	return;
}

void packet_buffer_init() {
	current_buffer = 0;
	for (int i = 0; i < PACKET_BUFFER_NUM; i++) {
		packet_buffers[i] = new pktinfo[PACKET_BUFFER_SIZE];
		if (packet_buffers[i] == 0) {
			exit(2);
		}
		packet_buffer_pointer[i] = 0;
	}
	return;
}

int init_connection() {

	sock_fd = socket( PF_NETLINK, SOCK_RAW, NETLINK_PACKET_INFO);
	if (sock_fd < 0)
		return (-1);

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
	recvmsg(sock_fd, &msg, 0);
	printf("Received message payload: %s\n", (char *) NLMSG_DATA(nlh));
	return 0;
}

void *storage_loop(void* ptr) {
	while (1) {
		int l;
		l = recvmsg(sock_fd, &msg, 0);
		for (int i = 0; i < l - 20; i += 20) {

			store_package((unsigned char *) NLMSG_DATA(nlh) + i);

		}
		//printf( "Received message payload: %d\n", l-16 );
	}
}

void* buffer_changer(void * ptr) {

	while (true) {
		sleep(1);
		//switch buffer variable
		//note that may cause racing condition

		current_buffer += 1;
		current_buffer %= PACKET_BUFFER_NUM;
		packet_buffer_pointer[current_buffer] = 0;
	}
	//call analysis threads
}

int main() {
	packet_buffer_init();
	if (init_connection() == -1)
		exit(1);
	pthread_t storage_t, buff_change_t;
	int ret_s = pthread_create(&storage_t, NULL, storage_loop, NULL);
	int ret_b = pthread_create(&buff_change_t, NULL, buffer_changer, NULL);
	//storage_loop(); //thread 1
	//buffer_changer(); //thread 2
	pthread_join(storage_t, NULL);
	pthread_join(buff_change_t, NULL);
	close(sock_fd);
}
