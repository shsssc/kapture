/*
This is a kernel module in the Kapture project(https://github.com/shsssc/kapture).
Please note the comments of "debug." 
Slight changes should be made before compiling for production environment in order to drop the unnecessary packets to other hosts after storing their headers for better performance.
*/ 


/*
 netlink is used in sending the headers to user space. This is the format of data.
 For better performance, these data will be sent every 10 packets.
 |sip	|dip	|sport	|dport	|len	|seq	|flag	|not used	|
 |32	|32	|16	|16	|16	|32	|8	|8     		|total 20 bytes;
 */



#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31
#define MSG_LEN 200

unsigned char sendbuff[MSG_LEN + 20];
unsigned char buffptr = 0;
#define NIPQUAD(addr) \
  ((unsigned char *)&addr)[0], \
  ((unsigned char *)&addr)[1], \
  ((unsigned char *)&addr)[2], \
  ((unsigned char *)&addr)[3]


// begin netlink
struct sock *nl_sk = NULL;
int pid = -1;

static void send_nl_msg(char* data, unsigned short len) {
	if (pid == -1)
		return;

	struct nlmsghdr *nlh;

	struct sk_buff *skb_out;
	int msg_size = len;
	int res;

	skb_out = nlmsg_new(msg_size, 0);

	if (!skb_out) {
		printk( KERN_ERR "Failed to allocate new skb\n" );
		return;
	}
	nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
	NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
	memcpy(nlmsg_data(nlh), data, msg_size);

	res = nlmsg_unicast(nl_sk, skb_out, pid);

	if (res < 0) {
		//printk( KERN_INFO "Error while sending bak to user\n" );
		//pass
	}
	return;
}
static void hello_nl_recv_msg(struct sk_buff *skb) {
	struct nlmsghdr *nlh;
	char *msg = "Hello from kernel";
	printk( KERN_INFO "Entering: %s\n", __FUNCTION__ );

	int msg_size = strlen(msg);

	nlh = (struct nlmsghdr *) skb->data;
	printk( KERN_INFO "Netlink received msg payload:%s\n", (char *) nlmsg_data( nlh ) );
	pid = nlh->nlmsg_pid; /*pid of sending process */
	send_nl_msg(msg, msg_size);

}

// begin hook
static unsigned int ipprint_func(unsigned int hooknum, struct sk_buff * skb,
		const struct net_device *in, const struct net_device *out,
		int (*okfn)(struct sk_buff *)) {
	__be32 sip, dip;
	unsigned int seq;

	struct tcphdr *tcph = NULL;
	struct udphdr *udph = NULL;
	unsigned short sport = 0;
	unsigned short dport = 0;
	unsigned short pkt_len = 0;
	unsigned char flags = 0;
	struct iphdr *iph;
	if (skb) {   
		if (0 &&/*!!debug*/ skb->pkt_type != PACKET_OTHERHOST)
			return NF_ACCEPT;
		struct sk_buff *sb = NULL;
		sb = skb;
		iph = ip_hdr(sb);
		sip = iph->saddr;
		dip = iph->daddr;
		pkt_len = ntohs(iph->tot_len);
		seq = 0;

		if (iph->protocol == IPPROTO_TCP) {
			tcph =
					(struct tcphdr *) ((char *) skb->data + (int) (iph->ihl * 4));
			//tcph = tcp_hdr(sb);
			sport = ntohs(tcph->source);
			dport = ntohs(tcph->dest);
			seq = ntohl(tcph->seq);
			flags = tcph - flags;
		} else if (iph->protocol == IPPROTO_UDP) {
			seq = 0;
			udph =
					(struct udphdr *) ((char *) skb->data + (int) (iph->ihl * 4));
			//udph = udp_hdr(sb);
			sport = ntohs(udph->source);
			dport = ntohs(udph->dest);
		}
		//printk("Packet for source address: %u.%u.%u.%u:%u destination address: %u.%u.%u.%u:%u len: %u seq: %u\n", NIPQUAD(sip),sport,NIPQUAD(dip),dport,pkt_len,seq);

		//saving to buffer
		sendbuff[buffptr] = ((unsigned char*) (&sip))[0];
		sendbuff[buffptr + 1] = ((unsigned char*) (&sip))[1];
		sendbuff[buffptr + 2] = ((unsigned char*) (&sip))[2];
		sendbuff[buffptr + 3] = ((unsigned char*) (&sip))[3];

		sendbuff[buffptr + 4] = ((unsigned char*) (&dip))[0];
		sendbuff[buffptr + 5] = ((unsigned char*) (&dip))[1];
		sendbuff[buffptr + 6] = ((unsigned char*) (&dip))[2];
		sendbuff[buffptr + 7] = ((unsigned char*) (&dip))[3];

		sendbuff[buffptr + 8] = ((unsigned char*) (&sport))[0];
		sendbuff[buffptr + 9] = ((unsigned char*) (&sport))[1];
		sendbuff[buffptr + 10] = ((unsigned char*) (&dport))[0];
		sendbuff[buffptr + 11] = ((unsigned char*) (&dport))[1];
		sendbuff[buffptr + 12] = ((unsigned char*) (&pkt_len))[0];
		sendbuff[buffptr + 13] = ((unsigned char*) (&pkt_len))[1];

		sendbuff[buffptr + 14] = ((unsigned char*) (&seq))[0];
		sendbuff[buffptr + 15] = ((unsigned char*) (&seq))[1];
		sendbuff[buffptr + 16] = ((unsigned char*) (&seq))[2];
		sendbuff[buffptr + 17] = ((unsigned char*) (&seq))[3];

		sendbuff[buffptr + 18] = flags;
		sendbuff[buffptr + 19] = 0;
		if (buffptr >= MSG_LEN - 20) {
			send_nl_msg(sendbuff, MSG_LEN);
			buffptr = 0;
		} else {
			buffptr += 20;
		}

	}
	return NF_ACCEPT;  //!!debug
	return NF_DROP;
}

struct nf_hook_ops ipprint_ops = { .list = { NULL, NULL }, .hook = ipprint_func,
		.pf = PF_INET, .hooknum = NF_INET_PRE_ROUTING,
		//.hooknum = NF_INET_LOCAL_IN,
		.priority = NF_IP_PRI_FILTER + 2 };

static int __init ipprint_init(void) {
	printk( "Entering: %s\n", __FUNCTION__ );
	nl_sk = netlink_kernel_create( &init_net, NETLINK_USER, 0, hello_nl_recv_msg, NULL, THIS_MODULE );
	if ( !nl_sk )
	{
		printk( KERN_ALERT "Error creating socket.\n" );
		return(-10);
	}

	nf_register_hook(&ipprint_ops);
	return 0;
}

static void __exit ipprint_exit(void) {
	printk( KERN_INFO "exiting capture module\n" );
	netlink_kernel_release( nl_sk );
	nf_unregister_hook(&ipprint_ops);
}

module_init (ipprint_init);
module_exit (ipprint_exit);
MODULE_AUTHOR("Sichen Song");
MODULE_DESCRIPTION("A kernel module for Traffic statistics");
MODULE_LICENSE("GPL");

