/*
 * PacketBuffer.cpp
 *
 *  Created on: Aug 13, 2017
 *      Author: root
 */

#include "PacketBuffer.h"

PacketBuffer::PacketBuffer() {
	current_buffer = 0;
	for (int i = 0; i < PACKET_BUFFER_NUM; i++) {
		packet_buffers[i] = new PktHdr[PACKET_BUFFER_SIZE];
		if (packet_buffers[i] == 0) {
			return;
		}
		packet_buffer_pointer[i] = 0;
	}
	return;

}

PacketBuffer::~PacketBuffer() {
	for (int i = 0; i < PACKET_BUFFER_NUM; i++) {
		delete[] packet_buffers[i];
	}
}

void PacketBuffer::switchBuffer() {
	current_buffer += 1;
	current_buffer %= PACKET_BUFFER_NUM;
	packet_buffer_pointer[current_buffer] = 0;
}

void PacketBuffer::storePacket(unsigned char* pkt) {
	unsigned char tmp_current_buffer = current_buffer;
	tmp_current_buffer = (tmp_current_buffer % PACKET_BUFFER_NUM); //dealing with a racing condition problem
	unsigned int& tmp_current_buffer_pointer =
			packet_buffer_pointer[tmp_current_buffer];

	PktHdr tmpPkt(pkt);

	packet_buffers[tmp_current_buffer][tmp_current_buffer_pointer++] = tmpPkt;

	/*printf("writing to buffer %u index %u\n", tmp_current_buffer,
	 tmp_current_buffer_pointer - 1);*/
	/*printf(
	 "Packet for source address: %u.%u.%u.%u:%u destination address: %u.%u.%u.%u:%u len: %u seq: %u\n",
	 NIPQUAD(tmpPkt.sip), tmpPkt.sport, NIPQUAD(tmpPkt.dip),
	 tmpPkt.dport, tmpPkt.pkt_len, tmpPkt.seq);
	 */
	return;
}
