#include "freebsd6.h"
#define freebsd6 freebsd6

#include <osreldate.h>
#define __FreeBSD_version __FreeBSD_kernel_version

#include <sys/queue.h>
#include <sys/_types.h>

typedef unsigned int tcp_seq;
typedef unsigned int tcp_cc;        
#define TCPOPT_SACK_HDR         (TCPOPT_NOP<<24|TCPOPT_NOP<<16|TCPOPT_SACK<<8)
#define MAX_SACK_BLKS   6       /* Max # SACK blocks stored at sender side */
#define TCP_MAX_SACK    3       /* MAX # SACKs sent in any segment */

#include <netinet/tcp_var.h>  

#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/in.h>

/*
 * Externalized form of struct socket used by the sysctl(3) interface.
 */
struct xsocket {
        size_t  xso_len;        /* length of this structure */
        struct  socket *xso_so; /* makes a convenient handle sometimes */
        short   so_type;
        short   so_options;
        short   so_linger;
        short   so_state;
        caddr_t so_pcb;         /* another convenient handle */
        int     xso_protocol;
        int     xso_family;
        short   so_qlen;
        short   so_incqlen;
        short   so_qlimit;
        short   so_timeo;
        u_short so_error;
        pid_t   so_pgid;
        u_long  so_oobmark;
        struct xsockbuf {
                u_int   sb_cc;
                u_int   sb_hiwat;
                u_int   sb_mbcnt;
                u_int   sb_mbmax;
                int     sb_lowat;
                int     sb_timeo;
                short   sb_flags;
        } so_rcv, so_snd;
        uid_t   so_uid;         /* XXX */
};
typedef     u_quad_t so_gen_t;

#define _SYS_SOCKETVAR_H_
#include <netinet/in_pcb.h>

struct  xtcpcb {
        size_t  xt_len;
        struct  inpcb   xt_inp;
        struct  tcpcb   xt_tp;
        struct  xsocket xt_socket;
        u_quad_t        xt_alignment_hack;
};

