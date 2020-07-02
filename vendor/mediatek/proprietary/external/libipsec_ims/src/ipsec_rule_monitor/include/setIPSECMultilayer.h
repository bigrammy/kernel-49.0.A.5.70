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
#ifndef _SETIPSECMULTILAYER_H
#define _SETIPSECMULTILAYER_H
#include <sys/un.h>
#include <linux/xfrm.h>
#include <linux/types.h>
#include <linux/socket.h>
#include <linux/rtnetlink.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "setkey_fileio/setkey_fileio.h"


typedef struct {

    unsigned int        local_spi_c;
    unsigned int        local_port_c;
    unsigned int        target_port_s;
    char        local_ip_c[256];
    char        target_ip_s[256];
    char        tunnel_ip_s[256];
    char        tunnel_ip_d[256];
    __be16      sport_mask; 
    __be16      dport_mask;
    int         ifindex;

    __u32               encry_algo;
    __u32               integrity_algo;

    short               is_wfc_enable;
    short               used;
    __u8                proto;
    __u8                dir;
    __u8                pref_s;
    __u8                pref_d;
    __u16               family;
    __u16               tunnel_family;
    unsigned int        request_id;          //< serial number of of inter-task request
    __u8                ipsec_type;
    __u32               priority;
    int                 user;   //volte_stack's pid


} siptx_req_ipsec_connect_struct;

#define RECORD_COUNT 32
#define ON 1
#define OFF 0

extern siptx_req_ipsec_connect_struct TunnelRule[RECORD_COUNT];
extern siptx_req_ipsec_connect_struct TransportRule[RECORD_COUNT];

extern int setkey_SP_tunnel_transport(char * src_range,char * dst_range,enum PROTOCOL_TYPE protocol,
					char * port_src,char * port_dst,char * src_tunnel,
					char * dst_tunnel,char * ipsec_type1,char * mode1, 
					char * ipsec_type2,char * mode2,char * direction,
					int u_id1,int u_id2);
extern int setkey_deleteSP(char * src,char * dst,enum PROTOCOL_TYPE protocol,char * src_port,
				char * dst_port,char * direction);
extern int setkey_SP_update_transport(char * src_range,char * dst_range,enum PROTOCOL_TYPE protocol,
				char * port_src,char * port_dst,char * ipsec_type, char * direction,
				int u_id);
extern int setkey_SP_update_tunnel_transport(char * src_range,char * dst_range,
				enum PROTOCOL_TYPE protocol,char * port_src,char * port_dst,
				char * src_tunnel,char * dst_tunnel,char * ipsec_type1,char * mode1, 
				char * ipsec_type2,char * mode2,char * direction,int u_id1,int u_id2);

int compre_ip(__u16 family,xfrm_address_t * srcip,xfrm_address_t * dstip);
int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len);

void ipsectostr(char * str, __u8 proto);
int deleteRuledatabase(siptx_req_ipsec_connect_struct * Ruledatabase,struct xfrm_userpolicy_id *xpid,
			char * srcbuf, char * dstbuf);
int compare_oldRuledatabase(siptx_req_ipsec_connect_struct * Ruledatabase,
			struct xfrm_userpolicy_info *xpinfo,char * srcbuf, char * dstbuf,
			char * srcbuf_tun, char * dstbuf_tun,struct xfrm_user_tmpl * tmpl);
int addNewRuledatabase(siptx_req_ipsec_connect_struct * Ruledatabase,struct xfrm_userpolicy_info *xpinfo,
			char * srcbuf, char * dstbuf,char * srcbuf_tun, char * dstbuf_tun,
			struct xfrm_user_tmpl * tmpl);
void set2layeripsecrules(siptx_req_ipsec_connect_struct * Tunndatabse,
			siptx_req_ipsec_connect_struct Transdata_var,__u8 dir,int Handover);
void set2layeripsecrules_xfrm(siptx_req_ipsec_connect_struct * Tunndatabse,
			siptx_req_ipsec_connect_struct Transdata_var,__u8 dir,int Handover);
void change1layeripsecrules(siptx_req_ipsec_connect_struct * Transdatabase,struct xfrm_userpolicy_id *xpid, 
			char * srcbuf, char * dstbuf);
void change1layeripsecrules_xfrm(siptx_req_ipsec_connect_struct * Transdatabase,
			struct xfrm_userpolicy_id *xpid, char * srcbuf, char * dstbuf);
int compare_ip_for_prefix(int family1, char* ip1, int pfix1, int family2, char* ip2, int pfix2);
#endif
