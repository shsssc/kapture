/*
 * PktHdr.h
 *
 *  Created on: Aug 13, 2017
 *      Author: root
 */

#ifndef PKTHDR_H_
#define PKTHDR_H_
#include <linux/netlink.h>
class PktHdr {
public:
	__be32 sip, dip;
	unsigned int seq;
	unsigned short sport, dport, pkt_len;
	unsigned char flags;
	PktHdr(unsigned char*);
	PktHdr();
	~PktHdr();
	bool synFlag();
	bool isTcp();
	unsigned long long makeKey();
};

#endif /* PKTHDR_H_ */
