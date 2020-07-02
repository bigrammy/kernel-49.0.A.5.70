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
 * based on iproute.c
 */
/*
 * Authors:
 *	Masahide NAKAMURA @USAGI
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <linux/netlink.h>
#include <linux/xfrm.h>
#include <linux/in.h>
#include <linux/in6.h>
#include "setIPSECMultilayer.h"
#include "utils.h"
#define LOG_TAG "ipsec_policy_mont"
#include <cutils/log.h>
#include <linux/capability.h>
#include <cutils/properties.h>



siptx_req_ipsec_connect_struct TunnelRule[RECORD_COUNT] = {{0,0,0,"\0","\0","\0","\0",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
siptx_req_ipsec_connect_struct TransportRule[RECORD_COUNT] = {{0,0,0,"\0","\0","\0","\0",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
extern int record_only;
#define IMSOVERLTE_WIFI "net.handover.rat"





static void xfrm_tmpl_process(struct xfrm_user_tmpl *tmpls, int len,
			    char * srcbuf, char * dstbuf, __u16 nlmsg_type,struct xfrm_userpolicy_info *xpinfo,struct xfrm_userpolicy_id *xpid)
{
	    char srcbuf_tun[256] = {0};
    char dstbuf_tun[256] = {0};	
	int ret = 0,ret_cmp =0,ret_add = -1;
	    int Twolayer_policy = 0,ret_del = -1;
	int ntmpls = len / sizeof(struct xfrm_user_tmpl);
    char overLTEorWifi[128] = { 0 };
      int i =0, transport_lable =-1;

	if(ntmpls > 1 && nlmsg_type != XFRM_MSG_DELPOLICY)
	{
		ALOGD("It is %d-layer ipsec rule\n",ntmpls);
		Twolayer_policy = 1;
	}
			rt_addr_ntoa(tmpls->family, &tmpls->saddr, srcbuf_tun, 256);
    		rt_addr_ntoa(tmpls->family, &tmpls->id.daddr, dstbuf_tun, 256);
	/*tunnel mode && (dst ip: ADDR_ANY|| src ip:ADDR_ANY)*/
  if(tmpls->mode == XFRM_MODE_TUNNEL )
  {

	  if(nlmsg_type == XFRM_MSG_DELPOLICY ) 
	  {
			
			ret_del = deleteRuledatabase(TunnelRule,xpid,srcbuf,dstbuf);
			//property_get(IMSOVERLTE_WIFI, overLTEorWifi, "");
			//ALOGD("get property--net.handover.rat:%s\n",overLTEorWifi);
			if((ret_del<RECORD_COUNT)&&(ret_del>-1)&&!record_only)
			{
				change1layeripsecrules_xfrm(TransportRule,xpid,srcbuf,dstbuf);  

			}

	  }
	  else 
	  {

			ret_cmp = compare_oldRuledatabase(TunnelRule,xpinfo,srcbuf, dstbuf,srcbuf_tun,dstbuf_tun,tmpls);
			if(ret_cmp ==-1) {	  
			  ret_add = addNewRuledatabase(TunnelRule,xpinfo,srcbuf, dstbuf,srcbuf_tun,dstbuf_tun,tmpls);			
			}
			
            		//property_get(IMSOVERLTE_WIFI, overLTEorWifi, "");
            		ALOGD("ret_cmp:%d,ret_add:%d,record_only:%d\n",ret_cmp,ret_add,record_only);
			if(!record_only)
            			if(ret_add >=0 &&ret_add <RECORD_COUNT) {
				/*lte handover wifi*/				
				set2layeripsecrules_xfrm(TransportRule,TunnelRule[ret_add],xpinfo->dir,1); 
					
				}
				#ifdef MOBIKE
				if((ret_cmp- RECORD_COUNT-1)>=0&&(ret_cmp- RECORD_COUNT-1) <RECORD_COUNT) 
				{
				/*wifi is on || need to update previous 2 layer transport-tunnel rules' tunnel ip*/				
				set2layeripsecrules_xfrm(TransportRule,TunnelRule[ret_cmp- RECORD_COUNT-1],xpinfo->dir,1);		
				}
				#endif
	  }

  }
    if(tmpls->mode == XFRM_MODE_TRANSPORT)
    {


	if(nlmsg_type == XFRM_MSG_DELPOLICY)
	{
		ALOGD("maybe it is previous TransportRule delete"); 
		deleteRuledatabase(TransportRule,xpid,srcbuf,dstbuf);
	}
	else
	{
		i =  compare_oldRuledatabase(TransportRule,xpinfo,srcbuf, dstbuf,srcbuf_tun,dstbuf_tun,tmpls);
		/*new records begin, record transportRule*/
		if(i ==-1) {

			  transport_lable =addNewRuledatabase(TransportRule,xpinfo,srcbuf, dstbuf,srcbuf_tun,dstbuf_tun,tmpls);			
		}
		if((transport_lable>=0)&&transport_lable<RECORD_COUNT)
		{
			ALOGD("It is a requested set [%d], begin to replace it with two-layer rules,src:%s/%u[%u],dst:%s/%u[%u],proto:%u,dir:%u,spi:%x,used:%d\n",transport_lable,TransportRule[transport_lable].local_ip_c,TransportRule[transport_lable].pref_s,TransportRule[transport_lable].local_port_c,TransportRule[transport_lable].target_ip_s,TransportRule[transport_lable].pref_d,TransportRule[transport_lable].target_port_s,TransportRule[transport_lable].proto,TransportRule[transport_lable].dir,TransportRule[transport_lable].local_spi_c,TransportRule[transport_lable].used);
			if((Twolayer_policy == 0)&&!record_only)
			{
        			set2layeripsecrules_xfrm(TunnelRule,TransportRule[transport_lable],xpinfo->dir,0);   
			}
		}
	}
    }
}

int xfrm_policy_flush(struct nlmsghdr *n)
{
  ALOGD("flush all policy\n");
  memset(TunnelRule, 0, sizeof(siptx_req_ipsec_connect_struct) *RECORD_COUNT);
  memset(TransportRule, 0, sizeof(siptx_req_ipsec_connect_struct) *RECORD_COUNT);
  return 1;
}

int xfrm_policy_record(struct nlmsghdr *n)
{
  ALOGD("record all policy\n");
  //memset(TunnelRule, 0, sizeof(siptx_req_ipsec_connect_struct) *RECORD_COUNT);
  //memset(TransportRule, 0, sizeof(siptx_req_ipsec_connect_struct) *RECORD_COUNT);
  return 1;
}



int xfrm_policy_process( struct nlmsghdr *n)
{
    char srcbuf[256] = {0};
    char dstbuf[256] = {0};
	int ret_del = -1;
	struct rtattr * tb[XFRMA_MAX+1];
	struct rtattr * rta = NULL,* rta_tmpl = NULL;
	struct xfrm_userpolicy_info *xpinfo = NULL;
	struct xfrm_user_polexpire *xpexp = NULL;
	struct xfrm_userpolicy_id *xpid = NULL;
	__u8 ptype = XFRM_POLICY_TYPE_MAIN;
	int len = n->nlmsg_len;
    __u16 f = AF_INET;

    char overLTEorWifi[128] = { 0 };


	if (n->nlmsg_type == XFRM_MSG_DELPOLICY)  {
		xpid = NLMSG_DATA(n);
		len -= NLMSG_SPACE(sizeof(*xpid));
		f = xpid->sel.family;
		rt_addr_ntoa(f, &xpid->sel.saddr, srcbuf, 256);
    rt_addr_ntoa(f, &xpid->sel.daddr, dstbuf, 256);
	} else if (n->nlmsg_type == XFRM_MSG_POLEXPIRE) {
		xpexp = NLMSG_DATA(n);
		xpinfo = &xpexp->pol;
		len -= NLMSG_SPACE(sizeof(*xpexp));
	} else {
		xpexp = NULL;
		xpinfo = NLMSG_DATA(n);
		len -= NLMSG_SPACE(sizeof(*xpinfo));
		    		f = xpinfo->sel.family;
    		rt_addr_ntoa(f, &xpinfo->sel.saddr, srcbuf, sizeof(srcbuf));
    		rt_addr_ntoa(f, &xpinfo->sel.daddr, dstbuf, sizeof(dstbuf));
	}
    ALOGD("parsePolicyMessage nh type:%d,len:%d,srcbuf:%s,dstbuf:%s,sizeof(sizeof(*xpinfo):%lu,f:%d,pid:%d\n", n->nlmsg_type,len,srcbuf,dstbuf,sizeof(*xpinfo),f,n->nlmsg_pid);
	if (len < 0) {
		ALOGE( "BUG: wrong nlmsg len %d\n", len);
		return -1;
	}

	if (n->nlmsg_type == XFRM_MSG_DELPOLICY)
		rta = XFRMPID_RTA(xpid);
	else if (n->nlmsg_type == XFRM_MSG_POLEXPIRE)
		rta = XFRMPEXP_RTA(xpexp);
	else
		rta = XFRMP_RTA(xpinfo);

	parse_rtattr(tb, XFRMA_MAX, rta, len);

  if(tb[XFRMA_TMPL]) {
  	rta_tmpl = tb[XFRMA_TMPL];
  	xfrm_tmpl_process((struct xfrm_user_tmpl *) RTA_DATA(rta_tmpl),RTA_PAYLOAD(rta), srcbuf,  dstbuf, n->nlmsg_type, xpinfo, xpid);

  }else {
  			ALOGD("There is no tmpl policy\n");
		if(n->nlmsg_type == XFRM_MSG_DELPOLICY)
		{
				deleteRuledatabase(TransportRule,xpid,srcbuf,dstbuf);
				ret_del = deleteRuledatabase(TunnelRule,xpid,srcbuf,dstbuf);
				//property_get(IMSOVERLTE_WIFI, overLTEorWifi, "");
				//ALOGD("get property--net.handover.rat:%s\n",overLTEorWifi);
				if((ret_del<RECORD_COUNT)&&(ret_del>-1)&&!record_only)
				{
					change1layeripsecrules_xfrm(TransportRule,xpid,srcbuf,dstbuf); 
				}

		}

		return 1;
  }

	//xfrm_policy_info_print(xpinfo, tb, fp, NULL, NULL);

	if (n->nlmsg_type == XFRM_MSG_POLEXPIRE) {

	}



	return 0;
}


