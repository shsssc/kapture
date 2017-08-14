/*
 * capture.h
 *
 *  Created on: Aug 11, 2017
 *      Author: root
 */

#ifndef CAPTURE_H_
#define CAPTURE_H_

#include "PktHdr.h"
#include "PacketBuffer.h"
#include "NetlinkConn.h"
#include "PacketHolder.h"
#include <thread>
void analysis_buffer(PktHdr * buffer, unsigned int len);
void storage_loop(NetlinkConn&);
void buffer_changer();
#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]

#endif /* CAPTURE_H_ */
