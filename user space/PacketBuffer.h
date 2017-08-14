/*
 * PacketBuffer.h
 *
 *  Created on: Aug 13, 2017
 *      Author: root
 */

#ifndef PACKETBUFFER_H_
#define PACKETBUFFER_H_
#include "PktHdr.h"
#include "stdio.h"
#define PACKET_BUFFER_SIZE 5000000
#define PACKET_BUFFER_NUM  3

#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]

class PacketBuffer {
public:
	PktHdr * packet_buffers[PACKET_BUFFER_NUM];
	unsigned char current_buffer;
	unsigned int packet_buffer_pointer[PACKET_BUFFER_NUM];
	PacketBuffer();
	~PacketBuffer();
	void switchBuffer();
	void storePacket(unsigned char* pkt);
};

#endif /* PACKETBUFFER_H_ */
