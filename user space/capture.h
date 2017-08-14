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
#include <thread>

void storage_loop(NetlinkConn&);
void buffer_changer();

#endif /* CAPTURE_H_ */
