#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

#include "utils_xfrm.h"
#include "setkey_xfrm_parse.h"



void rtnl_close_xfrm(struct rtnl_handle_xfrm *rth)
{
	if (rth->fd >= 0) {
		close(rth->fd);
		rth->fd = -1;
	}
}

int rtnl_open_byproto_xfrm(struct rtnl_handle_xfrm *rth, unsigned subscriptions,
		      int protocol)
{
	socklen_t addr_len;
	int sndbuf = 32768;

	memset(rth, 0, sizeof(*rth));

	rth->fd = socket(AF_NETLINK, SOCK_RAW, protocol);
	if (rth->fd < 0) {
		ALOGD("Cannot open netlink socket,errno:%d\n",errno);
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
		ALOGD("Cannot bind netlink socket\n");
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

int rtnl_listen_xfrm(struct rtnl_handle_xfrm *rtnl, rtnl_filter_t_xfrm handler)
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

	iov.iov_len = sizeof(buf);
		status = recvmsg(rtnl->fd, &msg, 0);
		ALOGD("netlink receive msg status:%d\n",status);
		if (status < 0) {
			if (errno == EINTR || errno == EAGAIN || errno == ENOBUFS)
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
			err = handler(rtnl,h);
			if (err == -2) /*no sa & sp entries*/
			{
				return err;
			}

			status -= NLMSG_ALIGN(len);
			h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
		}

	return 0;
}

/*
  -2: no previous volte_stack policy&& state is set;
  -1: unknown message type && delete policy or state failed
   0:  everything is good
*/
int rtnl_accept_msg_xfrm(struct rtnl_handle_xfrm * rth ,struct nlmsghdr *n)
{
	char pid[128] = {0};
	property_get("net.ims.volte.pid",pid,"-1");
	pid_t volte_pid =atoi(pid);
	/*no previous volte_stack policy&& state is set*/
	switch (n->nlmsg_type) {
	case XFRM_MSG_NEWSA:
	case XFRM_MSG_UPDSA:
		return xfrm_state_process_delete_exist( rth,n, volte_pid);
	case XFRM_MSG_NEWPOLICY:
	case XFRM_MSG_UPDPOLICY:
		return xfrm_policy_process_delete_exist( rth,n, volte_pid);
	case XFRM_MSG_EXPIRE:
	case XFRM_MSG_DELSA:
	case XFRM_MSG_FLUSHSA:
	case XFRM_MSG_GETPOLICY:
	case XFRM_MSG_FLUSHPOLICY:
	case XFRM_MSG_ACQUIRE:
	case XFRM_MSG_DELPOLICY:
	case XFRM_MSG_POLEXPIRE:
	default:
		ALOGD("receive netlink message: %08d 0x%08x 0x%08x\n",
			n->nlmsg_len, n->nlmsg_type, n->nlmsg_flags);
		break;
	}

	if (n->nlmsg_type != NLMSG_ERROR && n->nlmsg_type != NLMSG_NOOP &&
	    n->nlmsg_type != NLMSG_DONE) {
		ALOGE("Unknown message: %08d 0x%08x 0x%08x\n",
			n->nlmsg_len, n->nlmsg_type, n->nlmsg_flags);
	}
	return -2;
}

