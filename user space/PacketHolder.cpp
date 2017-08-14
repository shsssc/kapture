/*
 * PacketHolder.cpp
 *
 *  Created on: Aug 14, 2017
 *      Author: root
 */

#include "PacketHolder.h"

PacketHolder::PacketHolder() {
	// TODO Auto-generated constructor stub

}

PacketHolder::~PacketHolder() {
	// TODO Auto-generated destructor stub
}

void PacketHolder::insertPkt(PktHdr* pkt) {
	PKTHDR_VECTOR& tmpvct = this->operator[](pkt->makeKey());
	if (tmpvct.size() == 0) {
		tmpvct.reserve(20000);
	}
	tmpvct.push_back(pkt);
}

void PacketHolder::printTraffic() {
	PacketHolder::iterator i;
	unsigned long long total;
	for (i = this->begin(); i != this->end(); i++) {
		total = 0;
		for (unsigned int j = 0; j < i->second.size(); j++) {
			total += i->second[j]->pkt_len;
		}
		std::cout << "total: " << total << "\t";
	}
	std::cout << std::endl;
}

