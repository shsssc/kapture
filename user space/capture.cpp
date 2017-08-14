#include "capture.h"
#include "iostream"
/*
 * 暂时都是c语言，但是考虑数据结构可用stl所以用了c++
 * 序列号为0的是udp数据？？
 * todo: 修改内核模块把tcp的flag发过来
 * todo: 封装packet buffer
 * todo: 在切缓冲区的线程里面启动缓冲区分析线程
 * todo: 检查现有潜在竞争条件问题（主要在切缓冲区的时候），互斥体会不会影响性能？？
 * todo: 建立指向缓冲区包头指针的class，重载比较运算(源ip和目标ip小的放前面的unsigned long比较？？)，用stl对它们的数组/链表进行排序（稳定以不打乱时序）
 * todo: 对排序完成的数据顺序读取进行流量统计和丢包统计
 * todo: 对符合时序的原始缓冲区进行顺序读取以分析延迟（握手过程）
 *
 */

PacketBuffer* packet_buffer;

void storage_loop(NetlinkConn& nlConn) {
	while (1) {
		int l;
		l = nlConn.recvmsg();
		for (int i = 0; i < l - 20; i += 20) {

			packet_buffer->storePacket(nlConn.getMsg() + i);

		}
		//printf( "Received message payload: %d\n", l-16 );
	}
}

void buffer_changer() {

	while (true) {
		sleep(1);
		//switch buffer variable
		//note that may cause racing condition
		//todo: call analysis threads
		packet_buffer->switchBuffer();
	}

}

int main() {
	PktHdr test1;
	test1.sip=135792468;
	test1.dip=241241241;
	std::cout<<test1.makeKey()<<std::endl;
	while (1);
	packet_buffer = new PacketBuffer;
	NetlinkConn netlink_connection;
	std::thread t_storage(storage_loop, std::ref(netlink_connection));
	std::thread t_buffer_change(buffer_changer);
	t_storage.join();
	t_buffer_change.join();
}
