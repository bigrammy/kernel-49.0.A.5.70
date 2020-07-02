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
#include <endian.h>
#include <linux/xfrm.h>
#include <linux/in.h>
#include <linux/in6.h>
#define LOG_TAG "ipsec_policy_mont"
#include <cutils/log.h>
#include "utils.h"


int xfrm_state_process(struct nlmsghdr *n)
{

	struct rtattr * tb[XFRMA_MAX+1];
	struct rtattr * rta;
	struct xfrm_usersa_info *xsinfo = NULL;
	struct xfrm_user_expire *xexp = NULL;
	struct xfrm_usersa_id	*xsid = NULL;
	struct xfrm_usersa_flush *xsf = NULL;
	int len = n->nlmsg_len;

	if (n->nlmsg_type != XFRM_MSG_NEWSA &&
	    n->nlmsg_type != XFRM_MSG_DELSA &&
	    n->nlmsg_type != XFRM_MSG_UPDSA &&
	    n->nlmsg_type != XFRM_MSG_EXPIRE &&
	    n->nlmsg_type != XFRM_MSG_FLUSHSA) 
	    {
		ALOGE( "Not a state: %08x %08x %08x\n",
			n->nlmsg_len, n->nlmsg_type, n->nlmsg_flags);
		return 0;
	}

	if (n->nlmsg_type == XFRM_MSG_DELSA) {
		xsid = NLMSG_DATA(n);
		len -= NLMSG_SPACE(sizeof(*xsid));
	} else if (n->nlmsg_type == XFRM_MSG_EXPIRE) {
		xexp = NLMSG_DATA(n);
		xsinfo = &xexp->state;
		len -= NLMSG_SPACE(sizeof(*xexp));
	} else if (n->nlmsg_type == XFRM_MSG_FLUSHSA) {
		xsf = NLMSG_DATA(n);
		len -= NLMSG_SPACE(sizeof(*xsf));
	} else {
		xexp = NULL;
		xsinfo = NLMSG_DATA(n);
		len -= NLMSG_SPACE(sizeof(*xsinfo));
	}

	if (len < 0) {
		ALOGE("BUG: wrong nlmsg len %d\n", len);
		return -1;
	}


	if (n->nlmsg_type == XFRM_MSG_DELSA)
		ALOGD("Deleted ");
	else if (n->nlmsg_type == XFRM_MSG_UPDSA)
		ALOGD( "Updated ");
	else if (n->nlmsg_type == XFRM_MSG_EXPIRE)
		ALOGD( "Expired ");
	else if (n->nlmsg_type == XFRM_MSG_FLUSHSA)
		ALOGD( "FLUSHED State");
	else if (n->nlmsg_type == XFRM_MSG_NEWSA)
		ALOGD("new state");
				
#if 0
	if (n->nlmsg_type == XFRM_MSG_DELSA)
		rta = XFRMSID_RTA(xsid);
	else if (n->nlmsg_type == XFRM_MSG_EXPIRE)
		rta = XFRMEXP_RTA(xexp);
	else
		rta = XFRMS_RTA(xsinfo);

	parse_rtattr(tb, XFRMA_MAX, rta, len);

	if (n->nlmsg_type == XFRM_MSG_DELSA) {
		//xfrm_policy_id_print();

		if (!tb[XFRMA_SA]) {
			fprintf(stderr, "Buggy XFRM_MSG_DELSA: no XFRMA_SA\n");
			return -1;
		}
		if (RTA_PAYLOAD(tb[XFRMA_SA]) < sizeof(*xsinfo)) {
			fprintf(stderr, "Buggy XFRM_MSG_DELPOLICY: too short XFRMA_POLICY len\n");
			return -1;
		}
		xsinfo = RTA_DATA(tb[XFRMA_SA]);
	}

	xfrm_state_info_print(xsinfo, tb, fp, NULL, NULL);

	if (n->nlmsg_type == XFRM_MSG_EXPIRE) {
		fprintf(fp, "\t");
		fprintf(fp, "hard %u", xexp->hard);
		fprintf(fp, "%s", _SL_);
	}

	if (oneline)
		fprintf(fp, "\n");
	fflush(fp);
#endif
	return 0;
}

int xfrm_acquire_process( struct nlmsghdr *n)
{
	return 0;
}


