/* mv_nfp_mgr_if.h */

#ifndef MV_NFP_MGR_IF_H
#define MV_NFP_MGR_IF_H

#ifdef CONFIG_MV_ETH_NFP_FIB_LEARN
void nfp_fib_learn_enable(int en);
#endif

#ifdef CONFIG_MV_ETH_NFP_CT_LEARN
void nfp_ct_learn_enable(int en);
#endif

#ifdef CONFIG_MV_ETH_NFP_FDB_LEARN
void nfp_bridge_learn_enable(int en);
#endif

#ifdef CONFIG_MV_ETH_NFP_VLAN_LEARN
void nfp_vlan_learn_enable(int en);
#endif

#ifdef CONFIG_MV_ETH_NFP_PPP_LEARN
void nfp_ppp_learn_enable(int en);
#endif

extern rwlock_t nfp_lock;
#endif

