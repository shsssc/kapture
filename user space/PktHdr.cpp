/*
 * PktHdr.cpp
 *
 *  Created on: Aug 13, 2017
 *      Author: root
 */

#include "PktHdr.h"

PktHdr::PktHdr() {

}

PktHdr::PktHdr(unsigned char* pkt) {
	sip = *((__be32 *) (pkt));
	dip = *((__be32 *) (pkt + 4));
	sport = *((unsigned short *) (pkt + 8));
	dport = *((unsigned short *) (pkt + 10));
	pkt_len = *((unsigned short *) (pkt + 12));
	seq = *((unsigned int *) (pkt + 14));
	flags = *((unsigned char *) (pkt + 18));
}

PktHdr::~PktHdr() {
}

bool PktHdr::synFlag() {
	return flags & 0x02;
}

bool PktHdr::isTcp() {
	return (seq == 0) && (flags == 0);
}

unsigned long long PktHdr::makeKey(void) {
	if (sip < dip) {
		return ((unsigned long long) sip) * 4294967296 + dip;
	} else {
		return ((unsigned long long) dip) * 4294967296 + sip;
	}
}
