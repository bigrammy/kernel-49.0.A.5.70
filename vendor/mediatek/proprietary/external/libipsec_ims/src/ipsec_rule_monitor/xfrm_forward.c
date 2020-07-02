/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "netutils/ifc.h"
#include <sys/socket.h>
#include <linux/if.h>
#include <ifaddrs.h>
#include <cutils/log.h>
#include <linux/xfrm.h>
#include "utils.h"
#include "setIPSECMultilayer.h"
#define LOG_TAG "ipsec_policy_mont"


#define SIOCFWDFILTER (SIOCDEVPRIVATE+2)
#define IPV4 0x40
#define IPV6 0x60


typedef struct {

    char        local_ip_c[256];
    char        target_ip_s[256];
    short               used;
    __u8                pref_s;
    __u8                pref_d;
    __u8                cnt;
    __u16               family;
} ccmni_forward_ipsec_rule_struct;



typedef struct ccmni_fwd_filter {
	__u32 action;
	__u16 ver;
	__u8 s_pref;
	__u8 d_pref;
	union {
		struct {
			__u32 saddr;
			__u32 daddr;
		}ipv4;
		struct {
			__u32 saddr[4];
			__u32 daddr[4];
		}ipv6;
	}ip;
}ccmni_fwd_filter_t;

enum CCMNI_ACTION{
	CCMNI_FLT_ADD = 1,
	CCMNI_FLT_DEL,
	CCMNI_FLT_FLUSH
};

int sony_project = 1;
ccmni_forward_ipsec_rule_struct ForwardRule[RECORD_COUNT] = {{"\0","\0",0,0,0,0,0}};

int isIfaceName(const char *name) {
    size_t i;
    size_t name_len = strlen(name);
    if ((name_len == 0) || (name_len > IFNAMSIZ)) {
        return 0;
    }

    /* First character must be alphanumeric */
    if (!isalnum(name[0])) {
        return 0;
    }

    for (i = 1; i < name_len; i++) {
        if (!isalnum(name[i]) && (name[i] != '_') && (name[i] != '-') && (name[i] != ':')) {
            return 0;
        }
    }

    return 1;
}

//1 means matched, 0 means not match
int matchifbyaddr(struct sockaddr *sock_addr_a, char *addr_b, __u8 prefix_len, __u16 family)
{
	int ret;

	if(sock_addr_a == NULL || addr_b == NULL)
		return 0;
	if(sock_addr_a->sa_family != family)
		return 0;

	switch(sock_addr_a->sa_family) {
		case AF_INET: {
			struct sockaddr_in *sock_addr_in_a = (struct sockaddr_in *)sock_addr_a;
			struct in_addr sin_addr_b;
			inet_pton(AF_INET, addr_b, &sin_addr_b);
			ret = (sock_addr_in_a->sin_addr.s_addr - sin_addr_b.s_addr) ? 0 : 1;
			ALOGD("matchifbyaddr, family AF_INET, prefix_len %d, addr_a %x, addr_b %x\n", prefix_len, sock_addr_in_a->sin_addr, sin_addr_b);
			break;
			}
		case AF_INET6: {
			struct sockaddr_in6 *sock_addr_in6_a = (struct sockaddr_in6 *)sock_addr_a;
			struct in6_addr sin6_addr_b;
			inet_pton(AF_INET6, addr_b, &sin6_addr_b);
			ret = (sock_addr_in6_a->sin6_addr.s6_addr32[0] - sin6_addr_b.s6_addr32[0]) ||
				  (sock_addr_in6_a->sin6_addr.s6_addr32[1] - sin6_addr_b.s6_addr32[1]) ||
				  (sock_addr_in6_a->sin6_addr.s6_addr32[2] - sin6_addr_b.s6_addr32[2]) ||
				  (sock_addr_in6_a->sin6_addr.s6_addr32[3] - sin6_addr_b.s6_addr32[3]) ? 0 : 1;
			ALOGD("matchifbyaddr, family AF_INET6, prefix_len %d, addr_a %x_%x_%x_%x, addr_b %x_%x_%x_%x\n", prefix_len,
					sock_addr_in6_a->sin6_addr.s6_addr32[0], sock_addr_in6_a->sin6_addr.s6_addr32[1],
					sock_addr_in6_a->sin6_addr.s6_addr32[2], sock_addr_in6_a->sin6_addr.s6_addr32[3],
					sin6_addr_b.s6_addr32[0], sin6_addr_b.s6_addr32[1], sin6_addr_b.s6_addr32[2], sin6_addr_b.s6_addr32[3]);
			break;
			}
		default:
			return 0;
	}
	return ret;
}

//0 means get failed, 1 means get success
char record_ifname[IFNAMSIZ] = {'\0'};
int getifbyaddr(char *ifname, char *addr, __u8 prefix_len, __u16 family)
{
	struct ifaddrs *ifa_list, *ifa_iter;
	#define IPV4_PREFIX_LEN 32
	#define IPV6_PREFIX_LEN 128
	#define IF_HARD_CODE "ccmni4"
	if((family == AF_INET && prefix_len == IPV4_PREFIX_LEN) ||
		(family == AF_INET6 && prefix_len == IPV6_PREFIX_LEN)) {
		if(*record_ifname != '\0') {
			strncpy(ifname, record_ifname, strlen(record_ifname));
			return 1;
		}

		if(getifaddrs(&ifa_list) < 0) {
			ALOGE("getifaddrs failed(%s), ifname will be hard code as ccmni4\n", strerror(errno));
			strncpy(ifname, IF_HARD_CODE, strlen(IF_HARD_CODE) + 1);
			return 1;
		}
		for (ifa_iter = ifa_list; ifa_iter; ifa_iter = ifa_iter->ifa_next) {
			if(matchifbyaddr(ifa_iter->ifa_addr, addr, prefix_len, family)) {
				strncpy(ifname, ifa_iter->ifa_name, strlen(ifa_iter->ifa_name));
				ALOGD("getifbyaddr, ifname %s\n", ifname);
				freeifaddrs(ifa_list);
				return  1;
			}
		}
		freeifaddrs(ifa_list);
		ALOGE("matchifbyaddr failed\n");
		return 0;
	} else {
		ALOGE("family %u, prefix_len %u is not valid\n", family, prefix_len);
		return 0;
	}
}

void ifr_config(char *src, __u8 pref_s, char *dst, __u8 pref_d, __u16 family, char *interface, struct ifreq *ifr)
{
	ccmni_fwd_filter_t *ifr_ccmni_fwd_filter_p;

	ifr_ccmni_fwd_filter_p = (ccmni_fwd_filter_t *)ifr->ifr_ifru.ifru_data;
	strncpy(ifr->ifr_name, interface, IFNAMSIZ);
	ifr->ifr_name[IFNAMSIZ - 1] = '\0';
	ifr_ccmni_fwd_filter_p->s_pref = pref_s;
	ifr_ccmni_fwd_filter_p->d_pref = pref_d;
	switch(family) {
		case AF_INET:
		ifr_ccmni_fwd_filter_p->ver = IPV4;
		rt_addr_aton(family, src, (char *)&ifr_ccmni_fwd_filter_p->ip.ipv4.saddr);
		rt_addr_aton(family, dst, (char *)&ifr_ccmni_fwd_filter_p->ip.ipv4.daddr);
		ALOGD("ifr_ccmni_fwd_filter, ipv4, name %s, ver %u, pref_s %u, pref_d %u, saddr %x, daddr %x\n",
			ifr->ifr_name, ifr_ccmni_fwd_filter_p->ver, ifr_ccmni_fwd_filter_p->s_pref, ifr_ccmni_fwd_filter_p->d_pref,
			ifr_ccmni_fwd_filter_p->ip.ipv4.saddr, ifr_ccmni_fwd_filter_p->ip.ipv4.daddr);
		break;
		case AF_INET6:
		ifr_ccmni_fwd_filter_p->ver = IPV6;
		rt_addr_aton(family, src, (char *)ifr_ccmni_fwd_filter_p->ip.ipv6.saddr);
		rt_addr_aton(family, dst, (char *)ifr_ccmni_fwd_filter_p->ip.ipv6.daddr);
		ALOGD("ifr_ccmni_fwd_filter, ipv6, name %s, ver %u, pref_s %u, pref_d %u, saddr %x_%x_%x_%x, daddr %x_%x_%x_%x\n",
			ifr->ifr_name, ifr_ccmni_fwd_filter_p->ver, ifr_ccmni_fwd_filter_p->s_pref, ifr_ccmni_fwd_filter_p->d_pref,
			ifr_ccmni_fwd_filter_p->ip.ipv6.saddr[0], ifr_ccmni_fwd_filter_p->ip.ipv6.saddr[1],
			ifr_ccmni_fwd_filter_p->ip.ipv6.saddr[2], ifr_ccmni_fwd_filter_p->ip.ipv6.saddr[3],
			ifr_ccmni_fwd_filter_p->ip.ipv6.daddr[0], ifr_ccmni_fwd_filter_p->ip.ipv6.daddr[1],
			ifr_ccmni_fwd_filter_p->ip.ipv6.daddr[2], ifr_ccmni_fwd_filter_p->ip.ipv6.daddr[3]);
		break;
		default:
		ALOGE("ifreq config failed due to unknown family type\n");
		break;
	}
}

//0~31 means the old rule, -1 means the new rule
int forward_rule_isold(char *src, __u8 pref_s, char *dst, __u8 pref_d, __u16 family)
{
	ccmni_forward_ipsec_rule_struct * tmp = NULL;
	int new_rule;
	int i;

	for(i = 0; i < RECORD_COUNT;i++) {
		tmp = ForwardRule + i;
		if(tmp->used == 0)
			continue;
		 new_rule = (strncmp(tmp->local_ip_c, src, strlen(src))) || (strncmp(tmp->target_ip_s, dst, strlen(dst)))
					|| (tmp->pref_s != pref_s) || (tmp->pref_d!=pref_d) || (tmp->family!=family);
		if(!new_rule)
			return i;
	}

	ALOGI("src %s/%d, dst %s/%d, family %d is a new forward rule\n", src, pref_s, dst, pref_d, family);
	return -1;
}

void forward_rule_add_old(int i)
{
	ccmni_forward_ipsec_rule_struct * tmp = NULL;

	tmp = ForwardRule + i;
	++tmp->cnt;
	ALOGI("forward rule src %s/%d dst %s/%d has been added %d times\n", tmp->local_ip_c, tmp->pref_s, tmp->target_ip_s, tmp->pref_d, tmp->cnt);
}

//1 means delete success, 0 means delete fail
int forward_rule_del_old(int i)
{
	ccmni_forward_ipsec_rule_struct * tmp = NULL;

	tmp = ForwardRule + i;
	if(--tmp->cnt == 0) {
		memset(&ForwardRule[i], 0, sizeof(ccmni_forward_ipsec_rule_struct));
		ALOGI("forward rule is completely deleted\n");
		return 1;
	}
	ALOGI("forward rule src %s/%d dst %s/%d left %d times before deleted\n", tmp->local_ip_c, tmp->pref_s, tmp->target_ip_s, tmp->pref_d, tmp->cnt);
	return 0;
}

//0~31 means add rule success, -1 means fail
int forward_rule_add_new(char *src, __u8 pref_s, char *dst, __u8 pref_d, __u16 family)
{
	ccmni_forward_ipsec_rule_struct * tmp = NULL;
	int i;

	for(i=0; i < RECORD_COUNT; i++) {
		tmp = ForwardRule + i;
		if(tmp->used == 0) {
			memcpy(tmp->local_ip_c,src,strlen(src));
			memcpy(tmp->target_ip_s,dst,strlen(dst));
			tmp->pref_s = pref_s;
			tmp->pref_d = pref_d;
			tmp->family = family;
			tmp->used = 1;
			tmp->cnt = 1;
			ALOGI("src %s/%d, dst %s/%d, family %d has been added to forward rule\n", src, pref_s, dst, pref_d, family);
			return i;
		}
	}

	ALOGE("forward rule has run out of memory\n");
	return -1;
}


int forward_rule_count()
{
	ccmni_forward_ipsec_rule_struct * tmp = NULL;
	int cnt = 0;
	int i;

	for(i =0; i<RECORD_COUNT;i++) {
		tmp = ForwardRule + i;
		if(tmp->used == 1)
			cnt++;
	}
	return cnt;
}

int set_interface_forward(char * src, __u8 pref_s, char * dst,__u8 pref_d, __u16 family, char * interface, int enable)
{
	struct ifreq ifr;
	ccmni_fwd_filter_t ifr_ccmni_fwd_filter;
	int ifc_ctl_sock;

	if(!isIfaceName(interface)) {
		ALOGE("set interface %s forward failed\n", interface);
		return -ENOENT;
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	ifr.ifr_ifru.ifru_data = &ifr_ccmni_fwd_filter;
	ifr_config(src, pref_s, dst, pref_d, family, interface, &ifr);

	if(enable) {
		int channel;
		channel = forward_rule_isold(src, pref_s, dst, pref_d, family);
		if(channel >= 0) {
			ALOGI("forward rule is old and ioctl has been configured\n");
			forward_rule_add_old(channel);
			return 0;
		}

		(void)forward_rule_add_new(src, pref_s, dst, pref_d, family);
		if(forward_rule_count() == 1) {
			strncpy(record_ifname, interface, strlen(interface));
			if(ifc_set_ipsec_forward(interface, "wlan0", dst, pref_d, enable) == 0)
				ALOGD("enable ip forward success\n");
		}
	} else {
		int channel;
		channel = forward_rule_isold(src, pref_s, dst, pref_d, family);
		if(channel < 0) {
			ALOGI("forward rule did not be recorded and cannot be deleted\n");
			return 0;
		}

		if(forward_rule_del_old(channel) == 0) {
			ALOGI("forward rule has not been deleted actually\n");
			return 0;
		}
		if(forward_rule_count() == 0) {
			memset(record_ifname, '\0', strlen(record_ifname));
			if(ifc_set_ipsec_forward(interface, "wlan0",dst, pref_d,  enable) == 0)
				ALOGD("disable ip forward success\n");
		}
	}

	ifc_ctl_sock = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if(ifc_ctl_sock < 0) {
		ALOGE("ifc_ctl_sock create fail, %s\n", strerror(errno));
		return -errno;
	}
	ifr_ccmni_fwd_filter.action = enable ? CCMNI_FLT_ADD: CCMNI_FLT_DEL;
	if(ioctl(ifc_ctl_sock, SIOCFWDFILTER, &ifr) < 0) {
		ALOGE("%s interface forward ioctl %d failded, %s\n", enable? "enable" : "disable", SIOCFWDFILTER, strerror(errno));
		close(ifc_ctl_sock);
		return -errno;
	}
	ALOGD("ioctl: src:%s/%u,dst:%s/%u,family:%u from %s driver forward %s\n",src, pref_s, dst,pref_d, family, interface, enable ? "enable" : "disable");
	close(ifc_ctl_sock);
	return 0;
}
