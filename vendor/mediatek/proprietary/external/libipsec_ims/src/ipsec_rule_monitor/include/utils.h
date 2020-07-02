#ifndef __UTILS_H__
#define __UTILS_H__ 1
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_link.h>
#include <linux/if_addr.h>
#include <linux/neighbour.h>
#include <asm/types.h>
#include <stdlib.h>
#include <arpa/inet.h>


/*
 * Receiving buffer defines:
 * nlmsg
 *   data = struct xfrm_userpolicy_info
 *   rtattr
 *     data = struct xfrm_user_tmpl[]
 */
#define NLMSG_BUF_SIZE 4096
#define RTA_BUF_SIZE 2048
#define XFRM_TMPLS_BUF_SIZE 1024

struct rtnl_handle
{
	int			fd;
	struct sockaddr_nl	local;
	struct sockaddr_nl	peer;
	__u32			seq;
	__u32			dump;
};

struct req_handle {
	struct nlmsghdr			n;
	struct xfrm_userpolicy_info	xpinfo;
	char				buf[RTA_BUF_SIZE];
	} ;

typedef int (*rtnl_filter_t)(struct nlmsghdr *n);
extern int record_only;

#define NLMSG_TAIL(nmsg) \
	((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))
#define XFRMS_RTA(x)  ((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_usersa_info))))
#define XFRMS_PAYLOAD(n) NLMSG_PAYLOAD(n,sizeof(struct xfrm_usersa_info))

#define XFRMP_RTA(x)  ((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_userpolicy_info))))
#define XFRMP_PAYLOAD(n) NLMSG_PAYLOAD(n,sizeof(struct xfrm_userpoilcy_info))

#define XFRMSID_RTA(x)  ((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_usersa_id))))
#define XFRMSID_PAYLOAD(n) NLMSG_PAYLOAD(n,sizeof(struct xfrm_usersa_id))

#define XFRMPID_RTA(x)  ((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_userpolicy_id))))
#define XFRMPID_PAYLOAD(n) NLMSG_PAYLOAD(n,sizeof(struct xfrm_userpoilcy_id))

#define XFRMACQ_RTA(x)	((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_user_acquire))))
#define XFRMEXP_RTA(x)	((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_user_expire))))
#define XFRMPEXP_RTA(x)	((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_user_polexpire))))

#define XFRMREP_RTA(x)	((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(struct xfrm_user_report))))

#define XFRMSAPD_RTA(x)	((struct rtattr*)(((char*)(x)) + NLMSG_ALIGN(sizeof(__u32))))

extern char *rt_addr_ntoa(int af,const void *addr, char *buf, int buflen);
extern char *rt_addr_aton(int af,const void *srcaddr, char *dstbuf);

extern int rtnl_listen(struct rtnl_handle *, rtnl_filter_t handler);
extern int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len);
extern int rtnl_open(struct rtnl_handle *rth, unsigned subscriptions);
extern int rtnl_open_byproto(struct rtnl_handle *rth, unsigned subscriptions, int protocol);
extern void rtnl_close(struct rtnl_handle *rth);
extern int xfrm_state_process( struct nlmsghdr *n);
extern int xfrm_policy_flush(struct nlmsghdr *n);
extern int xfrm_policy_process(struct nlmsghdr *n);
extern int xfrm_acquire_process(struct nlmsghdr *n);
extern int xfrm_policy_record(struct nlmsghdr *n);
extern int rtnl_talk(struct rtnl_handle *rtnl, struct nlmsghdr *n, 
	      unsigned groups, struct nlmsghdr *answer);
extern int sony_project;
extern int set_interface_forward(char * src, __u8 pref_s, char * dst,__u8 pref_d, __u16 family, char * interface, int enable);
extern int getifbyaddr(char *ifname, char *addr, __u8 prefix_len, __u16 family);
#endif /* __UTILS_H__ */
