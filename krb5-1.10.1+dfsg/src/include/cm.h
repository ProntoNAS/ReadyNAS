/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* include/cm.h */
/*
 * Copyright 2002 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Export of this software from the United States of America may
 *   require a specific license from the United States Government.
 *   It is the responsibility of any person or organization contemplating
 *   export to obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  Furthermore if you modify this software you must label
 * your software as modified software and not distribute it in such a
 * fashion that it might be confused with the original M.I.T. software.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 */

/*
 * Since fd_set is large on some platforms (8K on AIX 5.2), this probably
 * shouldn't be allocated in automatic storage.  Define USE_POLL and
 * MAX_POLLFDS in the consumer of this header file to use poll state instead of
 * select state.
 */
struct select_state {
#ifdef USE_POLL
    struct pollfd fds[MAX_POLLFDS];
#else
    int max;
    fd_set rfds, wfds, xfds;
#endif
    int nfds;
    struct timeval end_time;    /* magic: tv_sec==0 => never time out */
};


/* Select state flags.  */
#define SSF_READ        0x01
#define SSF_WRITE       0x02
#define SSF_EXCEPTION   0x04


static const char *const state_strings[] = {
    "INITIALIZING", "CONNECTING", "WRITING", "READING", "FAILED"
};


/* connection states */
enum conn_states { INITIALIZING, CONNECTING, WRITING, READING, FAILED };
struct incoming_krb5_message {
    size_t bufsizebytes_read;
    size_t bufsize;
    char *buf;
    char *pos;
    unsigned char bufsizebytes[4];
    size_t n_left;
};
struct conn_state {
    SOCKET fd;
    krb5_error_code err;
    enum conn_states state;
    unsigned int is_udp : 1;
    int (*service)(krb5_context context, struct conn_state *,
                   struct select_state *, int);
    int socktype;
    int family;
    size_t addrlen;
    struct sockaddr_storage addr;
    struct {
        struct {
            sg_buf sgbuf[2];
            sg_buf *sgp;
            int sg_count;
            unsigned char msg_len_buf[4];
        } out;
        struct incoming_krb5_message in;
    } x;
    krb5_data callback_buffer;
    size_t server_index;
    struct conn_state *next;
};

struct sendto_callback_info {
    int  (*pfn_callback) (struct conn_state *, void *, krb5_data *);
    void (*pfn_cleanup)  (void *, krb5_data *);
    void  *context;
};


krb5_error_code krb5int_cm_call_select (const struct select_state *,
                                        struct select_state *, int *);
