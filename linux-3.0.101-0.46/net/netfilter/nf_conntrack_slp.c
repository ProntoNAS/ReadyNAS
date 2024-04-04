/*
 *      NetBIOS name service broadcast connection tracking helper
 *
 *      (c) 2007 Jiri Bohac <jbohac@suse.cz>
 *      (c) 2005 Patrick McHardy <kaber@trash.net>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */
/*
 *      This helper tracks locally originating NetBIOS name service
 *      requests by issuing permanent expectations (valid until
 *      timing out) matching all reply connections from the
 *      destination network. The only NetBIOS specific thing is
 *      actually the port number.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/if_addr.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <net/route.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_expect.h>

#define SLP_PORT	427

MODULE_AUTHOR("Jiri Bohac <jbohac@suse.cz>");
MODULE_DESCRIPTION("SLP broadcast connection tracking helper");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ip_conntrack_slp");

static unsigned int timeout __read_mostly = 3;
module_param(timeout, uint, 0400);
MODULE_PARM_DESC(timeout, "timeout for master connection/replies in seconds");

static int help(struct sk_buff *skb, unsigned int protoff,
		struct nf_conn *ct, enum ip_conntrack_info ctinfo)
{
	struct nf_conntrack_expect *exp;
	struct rtable *rt = skb_rtable(skb);
	struct in_device *in_dev;
	__be32 mask = 0;
	__be32 src = 0;

	/* we're only interested in locally generated packets */
	if (skb->sk == NULL)
		goto out;
	if (rt == NULL || !(rt->rt_flags & (RTCF_MULTICAST|RTCF_BROADCAST)))
		goto out;
	if (CTINFO2DIR(ctinfo) != IP_CT_DIR_ORIGINAL)
		goto out;

	rcu_read_lock();
	in_dev = __in_dev_get_rcu(rt->dst.dev);
	if (in_dev != NULL) {
		for_primary_ifa(in_dev) {
			/* this is a hack as slp uses multicast we can't match
			 * the destination address to some broadcast address. So
			 * just take the first one. Better would be to install
			 * expectations for all addresses */
			mask = ifa->ifa_mask;
			src = ifa->ifa_broadcast;
			break;
		} endfor_ifa(in_dev);
	}
	rcu_read_unlock();

	if (mask == 0 || src == 0)
		goto out;

	exp = nf_ct_expect_alloc(ct);
	if (exp == NULL)
		goto out;

	exp->tuple                = ct->tuplehash[IP_CT_DIR_REPLY].tuple;
	exp->tuple.src.u3.ip      = src;
	exp->tuple.src.u.udp.port = htons(SLP_PORT);

	exp->mask.src.u3.ip       = mask;
	exp->mask.src.u.udp.port  = htons(0xFFFF);

	exp->expectfn             = NULL;
	exp->flags                = NF_CT_EXPECT_PERMANENT;
	exp->class		  = NF_CT_EXPECT_CLASS_DEFAULT;
	exp->helper               = NULL;

	nf_ct_expect_related(exp);
	nf_ct_expect_put(exp);

	nf_ct_refresh(ct, skb, timeout * HZ);
out:
	return NF_ACCEPT;
}

static struct nf_conntrack_expect_policy exp_policy = {
	.max_expected	= 1,
};

static struct nf_conntrack_helper helper __read_mostly = {
	.name			= "slp",
	.tuple.src.l3num	= AF_INET,
	.tuple.src.u.udp.port	= __constant_htons(SLP_PORT),
	.tuple.dst.protonum	= IPPROTO_UDP,
	.me			= THIS_MODULE,
	.help			= help,
	.expect_policy		= &exp_policy,
};

static int __init nf_conntrack_slp_init(void)
{
	exp_policy.timeout = timeout;
	return nf_conntrack_helper_register(&helper);
}

static void __exit nf_conntrack_slp_fini(void)
{
	nf_conntrack_helper_unregister(&helper);
}

module_init(nf_conntrack_slp_init);
module_exit(nf_conntrack_slp_fini);
