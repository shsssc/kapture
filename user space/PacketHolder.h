/*
 * PacketHolder.h
 *
 *  Created on: Aug 14, 2017
 *      Author: root
 */

#ifndef PACKETHOLDER_H_
#define PACKETHOLDER_H_
#include "iostream"
#include <map>
#include <vector>
#include "PktHdr.h"
typedef std::vector<PktHdr*> PKTHDR_VECTOR;
typedef std::map<unsigned long long,PKTHDR_VECTOR> LONGLONG_VECTOR_MAP;

class PacketHolder:private LONGLONG_VECTOR_MAP{
public:
	PacketHolder();
	void insertPkt(PktHdr& pkt);
	void insertPkt(PktHdr* pkt);
	~PacketHolder();
	void printTraffic();
};

#endif /* PACKETHOLDER_H_ */
