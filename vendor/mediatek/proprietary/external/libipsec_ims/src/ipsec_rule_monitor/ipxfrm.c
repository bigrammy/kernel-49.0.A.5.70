/* $USAGI: $ */

/*
 * Copyright (C)2004 USAGI/WIDE Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * based on ip.c, iproute.c
 */
/*
 * Authors:
 *	Masahide NAKAMURA @USAGI
 */

#include <stdio.h>
#include <linux/errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <endian.h>
#include <time.h>
#include <netdb.h>
#include <linux/netlink.h>
#include <linux/if_link.h>
#include <linux/rtnetlink.h>
#include <linux/xfrm.h>
#include <linux/ipsec.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <arpa/inet.h>
#include "setIPSECMultilayer.h"
#define LOG_TAG "ipsec_policy_mont"
#include <cutils/log.h>
#include "utils.h"
#include <cutils/properties.h>

struct rtnl_handle rth = { -1,{0},{0},0,0 };
int record_only = 1;


/* This is based on utils.c(inet_addr_match) */
int xfrm_addr_match(xfrm_address_t *x1, xfrm_address_t *x2, int bits)
{
	__u32 *a1 = (__u32 *)x1;
	__u32 *a2 = (__u32 *)x2;
	int words = bits >> 0x05;

	bits &= 0x1f;

	if (words)
		if (memcmp(a1, a2, words << 2))
			return -1;

	if (bits) {
		__u32 w1, w2;
		__u32 mask;

		w1 = a1[words];
		w2 = a2[words];

		mask = htonl((0xffffffff) << (0x20 - bits));

		if ((w1 ^ w2) & mask)
			return 1;
	}

	return 0;
}

int xfrm_xfrmproto_is_ipsec(__u8 proto)
{
	return (proto ==  IPPROTO_ESP ||
		proto ==  IPPROTO_AH  ||
		proto ==  IPPROTO_COMP);
}


struct typeent {
	const char *t_name;
	int t_type;
};

static const struct typeent xfrmproto_types[]= {
	{ "esp", IPPROTO_ESP }, { "ah", IPPROTO_AH }, { "comp", IPPROTO_COMP },
	{ "route2", IPPROTO_ROUTING }, { "hao", IPPROTO_DSTOPTS },
	{ "ipsec-any", IPSEC_PROTO_ANY },
	{ NULL, -1 }
};

int xfrm_xfrmproto_getbyname(char *name)
{
	int i;

	for (i = 0; ; i++) {
		const struct typeent *t = &xfrmproto_types[i];
		if (!t->t_name || t->t_type == -1)
			break;

		if (strcmp(t->t_name, name) == 0)
			return t->t_type;
	}

	return -1;
}




static int xfrm_selector_iszero(struct xfrm_selector *s)
{
	struct xfrm_selector s0;

	memset(&s0, 0, sizeof(s0));

	return (memcmp(&s0, s, sizeof(s0)) == 0);
}

int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
	memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
	while (RTA_OK(rta, len)) {
		if ((rta->rta_type <= max) && (!tb[rta->rta_type]))
			tb[rta->rta_type] = rta;
		rta = RTA_NEXT(rta,len);
	}
	if (len)
		fprintf(stderr, "!!!Deficit %d, rta_len=%d\n", len, rta->rta_len);
	return 0;
}



static inline __u32 nl_mgrp(__u32 group)
{
	if (group > 31 ) {
		ALOGD("Use setsockopt for this group %d\n", group);
		return -1;
	}
	return group ? (1 << (group - 1)) : 0;
}
int rtnl_wilddump_request(struct rtnl_handle *rth, int family, int type)
{
	struct {
		struct nlmsghdr nlh;
		struct rtgenmsg g;
		__u16 align_rta;	/* attribute has to be 32bit aligned */
		struct rtattr ext_req;
		__u32 ext_filter_mask;
	} req;

	memset(&req, 0, sizeof(req));
	req.nlh.nlmsg_len = sizeof(req);
	req.nlh.nlmsg_type = type;
	req.nlh.nlmsg_flags = NLM_F_DUMP|NLM_F_REQUEST;
	req.nlh.nlmsg_pid = getpid();
	req.nlh.nlmsg_seq = rth->dump = ++rth->seq;
	req.g.rtgen_family = family;

	req.ext_req.rta_type = IFLA_EXT_MASK;
	req.ext_req.rta_len = RTA_LENGTH(sizeof(__u32));
	req.ext_filter_mask = RTEXT_FILTER_VF;

	return send(rth->fd, (void*)&req, sizeof(req), 0);
}


int rcvbuf = 1024 * 1024;

void rtnl_close(struct rtnl_handle *rth)
{
	if (rth->fd >= 0) {
		close(rth->fd);
		rth->fd = -1;
	}
}

int rtnl_open_byproto(struct rtnl_handle *rth, unsigned subscriptions,
		      int protocol)
{
	socklen_t addr_len;
	int sndbuf = 32768;

	memset(rth, 0, sizeof(*rth));

	rth->fd = socket(AF_NETLINK, SOCK_RAW, protocol);
	if (rth->fd < 0) {
		ALOGD("Cannot open netlink socket,errno:%d",errno);
		return -1;
	}

	/*if (setsockopt(rth->fd,SOL_SOCKET,SO_SNDBUF,&sndbuf,sizeof(sndbuf)) < 0) {
		perror("SO_SNDBUF");
		return -1;
	}

	if (setsockopt(rth->fd,SOL_SOCKET,SO_RCVBUF,&rcvbuf,sizeof(rcvbuf)) < 0) {
		perror("SO_RCVBUF");
		return -1;
	}*/

	memset(&rth->local, 0, sizeof(rth->local));
	rth->local.nl_family = AF_NETLINK;
	rth->local.nl_groups = subscriptions;

	if (bind(rth->fd, (struct sockaddr*)&rth->local, sizeof(rth->local)) < 0) {
		ALOGD("Cannot bind netlink socket");
		return -1;
	}
	/*addr_len = sizeof(rth->local);
	if (getsockname(rth->fd, (struct sockaddr*)&rth->local, &addr_len) < 0) {
		perror("Cannot getsockname");
		return -1;
	}
	if (addr_len != sizeof(rth->local)) {
		fprintf(stderr, "Wrong address length %d\n", addr_len);
		return -1;
	}
	if (rth->local.nl_family != AF_NETLINK) {
		fprintf(stderr, "Wrong address family %d\n", rth->local.nl_family);
		return -1;
	}
	rth->seq = time(NULL);*/
	return 0;
}



int rtnl_listen(struct rtnl_handle *rtnl, rtnl_filter_t handler)
{
	int status;
	struct nlmsghdr *h;
	struct sockaddr_nl nladdr;
	struct iovec iov;
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};
	char   buf[16384];

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = 0;
	nladdr.nl_groups = 0;
	iov.iov_base = buf;



	while (1) {
		iov.iov_len = sizeof(buf);
		status = recvmsg(rtnl->fd, &msg, 0);

		if (status < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			ALOGD("netlink receive error %s (%d)\n",strerror(errno), errno);
			if (errno == ENOBUFS)
				continue;
			return -1;
		}
		if (status == 0) {
			ALOGE("EOF on netlink\n");
			return -1;
		}
		if (msg.msg_namelen != sizeof(nladdr)) {
			ALOGE("Sender address length == %d\n", msg.msg_namelen);
			return -1;
		}
		for (h = (struct nlmsghdr*)buf; status >= sizeof(*h); ) {
			int err;
			int len = h->nlmsg_len;
			int l = len - sizeof(*h);

			if (l<0 || len>status) {
				if (msg.msg_flags & MSG_TRUNC) {
					ALOGE("Truncated message\n");
					return -1;
				}
				ALOGE(stderr, "!!!malformed message: len=%d\n", len);
				return -1;
			}
                        if(h->nlmsg_pid != getpid()) {		
				record_only = 0;
				//ALOGD( "message from other process:%d\n",h->nlmsg_pid);
			} else
				record_only = 1;		
			err = handler(h);
			if (err < 0)
				return err;

			status -= NLMSG_ALIGN(len);
			h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
		}
		if (msg.msg_flags & MSG_TRUNC) {
			ALOGD( "Message truncated\n");
			continue;
		}
		if (status) {
			ALOGE("!!!Remnant of size %d\n", status);
			return -1;
		}
	}
}

int rtnl_talk(struct rtnl_handle *rtnl, struct nlmsghdr *n,
	      unsigned groups, struct nlmsghdr *answer)
{
	int status;
	unsigned seq;
	struct nlmsghdr *h;
	struct sockaddr_nl nladdr;
	struct iovec iov = {
		.iov_base = (void*) n,
		.iov_len = n->nlmsg_len
	};
	struct msghdr msg = {
		.msg_name = &nladdr,
		.msg_namelen = sizeof(nladdr),
		.msg_iov = &iov,
		.msg_iovlen = 1,
	};


	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = getpid();
	nladdr.nl_groups = groups;

	n->nlmsg_seq = seq = ++rtnl->seq;

	if (answer == NULL)
		n->nlmsg_flags |= NLM_F_ACK;

	status = sendmsg(rtnl->fd, &msg, 0);

	if (status < 0) {
		ALOGD("Cannot talk to rtnetlink");
		return -1;
	}
	ALOGD("rtnl_talk fd:%d,status:%d,n->nlmsg_len:%d,n->nlmsg_seq:%d,errno:%d",rtnl->fd,status,n->nlmsg_len,n->nlmsg_seq,errno);
	return 0;
}


static int xfrm_accept_msg(struct nlmsghdr *n)
{


  //ALOGD("xfrm_accept_msg type:%d,fd:%d ",n->nlmsg_type,rth.fd);
	switch (n->nlmsg_type) {
	case XFRM_MSG_NEWSA:
	case XFRM_MSG_DELSA:
	case XFRM_MSG_UPDSA:
	case XFRM_MSG_EXPIRE:
	case XFRM_MSG_FLUSHSA:
		xfrm_state_process( n);
		return 0;
	case XFRM_MSG_NEWPOLICY:
	case XFRM_MSG_DELPOLICY:
	case XFRM_MSG_UPDPOLICY:
	case XFRM_MSG_POLEXPIRE:
		xfrm_policy_process( n);
		return 0;
	case XFRM_MSG_GETPOLICY:
		xfrm_policy_record( n);
		return 0;
	case XFRM_MSG_FLUSHPOLICY:

		xfrm_policy_flush( n);
		return 0;
	case XFRM_MSG_ACQUIRE:
		xfrm_acquire_process( n);
	default:
		break;
	}

	if (n->nlmsg_type != NLMSG_ERROR && n->nlmsg_type != NLMSG_NOOP &&
	    n->nlmsg_type != NLMSG_DONE) {
		ALOGE("Unknown message: %08d 0x%08x 0x%08x\n",
			n->nlmsg_len, n->nlmsg_type, n->nlmsg_flags);
	}
	return 0;
}


int main(void)
{
	int ret_send = 0;
	char version[128] = {0};
	unsigned groups = ~((unsigned)0); /* XXX */
  //memset(&filter, 0, sizeof(filter));
	rtnl_close(&rth);

	groups |= (nl_mgrp(XFRMNLGRP_ACQUIRE)|nl_mgrp(XFRMNLGRP_EXPIRE)|nl_mgrp(XFRMNLGRP_SA)|nl_mgrp(XFRMNLGRP_POLICY));

	if (rtnl_open_byproto(&rth, groups, NETLINK_XFRM) < 0)
		exit(1);		
  int ret = property_set("net.ims.ipsec.version","2.0");
  if(ret != 0)
  {
      ALOGE("set property failed,errno:%d\n",errno);
      exit(1);
  }
  else
  {
    property_get("net.ims.ipsec.version",version,"");
    ALOGD("getproperty-- net.ims.ipsec.version :%s\n",version);
  }
  /*dump previous policy rules*/
  ret_send = rtnl_wilddump_request(&rth, AF_UNSPEC, XFRM_MSG_GETPOLICY);
  if(ret_send < 0)
  {
	    ALOGE("send error,errno:%d\n",errno); 
	} else {
	    ALOGD("send success to get previous policy,ret_send:%d\n",ret_send);
	}

	if (rtnl_listen(&rth, xfrm_accept_msg) < 0)
		exit(2);

	return 0;
}
