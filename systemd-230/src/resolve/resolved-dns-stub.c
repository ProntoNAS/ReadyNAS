/***
  This file is part of systemd.

  Copyright 2016 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include "fd-util.h"
#include "resolved-dns-stub.h"
#include "socket-util.h"

/* The MTU of the loopback device is 64K on Linux, advertise that as maximum datagram size, but subtract the Ethernet,
 * IP and UDP header sizes */
#define ADVERTISE_DATAGRAM_SIZE_MAX (65536U-14U-20U-8U)

static int dns_stub_make_reply_packet(
                uint16_t id,
                int rcode,
                DnsQuestion *q,
                DnsAnswer *answer,
                bool add_opt,   /* add an OPT RR to this packet */
                bool edns0_do,  /* set the EDNS0 DNSSEC OK bit */
                bool ad,        /* set the DNSSEC authenticated data bit */
                DnsPacket **ret) {

        _cleanup_(dns_packet_unrefp) DnsPacket *p = NULL;
        DnsResourceRecord *rr;
        unsigned c = 0;
        int r;

        /* Note that we don't bother with any additional RRs, as this is stub is for local lookups only, and hence
         * roundtrips aren't expensive. */

        r = dns_packet_new(&p, DNS_PROTOCOL_DNS, 0);
        if (r < 0)
                return r;

        /* If the client didn't do EDNS, clamp the rcode to 4 bit */
        if (!add_opt && rcode > 0xF)
                rcode = DNS_RCODE_SERVFAIL;

        DNS_PACKET_HEADER(p)->id = id;
        DNS_PACKET_HEADER(p)->flags = htobe16(DNS_PACKET_MAKE_FLAGS(
                                                              1 /* qr */,
                                                              0 /* opcode */,
                                                              0 /* aa */,
                                                              0 /* tc */,
                                                              1 /* rd */,
                                                              1 /* ra */,
                                                              ad /* ad */,
                                                              0 /* cd */,
                                                              rcode));

        r = dns_packet_append_question(p, q);
        if (r < 0)
                return r;
        DNS_PACKET_HEADER(p)->qdcount = htobe16(dns_question_size(q));

        DNS_ANSWER_FOREACH(rr, answer) {
                r = dns_question_matches_rr(q, rr, NULL);
                if (r < 0)
                        return r;
                if (r > 0)
                        goto add;

                r = dns_question_matches_cname_or_dname(q, rr, NULL);
                if (r < 0)
                        return r;
                if (r > 0)
                        goto add;

                continue;
        add:
                r = dns_packet_append_rr(p, rr, NULL, NULL);
                if (r < 0)
                        return r;

                c++;
        }
        DNS_PACKET_HEADER(p)->ancount = htobe16(c);

        if (add_opt) {
                r = dns_packet_append_opt(p, ADVERTISE_DATAGRAM_SIZE_MAX, edns0_do, rcode, NULL);
                if (r < 0)
                        return r;
        }

        *ret = p;
        p = NULL;

        return 0;
}

static void dns_stub_detach_stream(DnsStream *s) {
        assert(s);

        s->complete = NULL;
        s->on_packet = NULL;
        s->query = NULL;
}

static int dns_stub_send(Manager *m, DnsStream *s, DnsPacket *p, DnsPacket *reply) {
        int r;

        assert(m);
        assert(p);
        assert(reply);

        if (s)
                r = dns_stream_write_packet(s, reply);
        else {
                int fd;

                /* Truncate the message to the right size */
                if (reply->size > DNS_PACKET_PAYLOAD_SIZE_MAX(p)) {
                        dns_packet_truncate(reply, DNS_PACKET_UNICAST_SIZE_MAX);
                        DNS_PACKET_HEADER(reply)->flags = htobe16(be16toh(DNS_PACKET_HEADER(reply)->flags) | DNS_PACKET_FLAG_TC);
                }

                fd = manager_dns_stub_udp_fd(m);
                if (fd < 0)
                        return log_debug_errno(fd, "Failed to get reply socket: %m");

                /* Note that it is essential here that we explicitly choose the source IP address for this packet. This
                 * is because otherwise the kernel will choose it automatically based on the routing table and will
                 * thus pick 127.0.0.1 rather than 127.0.0.53. */

                r = manager_send(m, fd, LOOPBACK_IFINDEX, p->family, &p->sender, p->sender_port, &p->destination, reply);
        }
        if (r < 0)
                return log_debug_errno(r, "Failed to send reply packet: %m");

        return 0;
}

static int dns_stub_send_failure(Manager *m, DnsStream *s, DnsPacket *p, int rcode) {
        _cleanup_(dns_packet_unrefp) DnsPacket *reply = NULL;
        int r;

        assert(m);
        assert(p);

        r = dns_stub_make_reply_packet(DNS_PACKET_ID(p), rcode, p->question, NULL, !!p->opt, DNS_PACKET_DO(p), false, &reply);
        if (r < 0)
                return log_debug_errno(r, "Failed to build failure packet: %m");

        return dns_stub_send(m, s, p, reply);
}

static void dns_stub_query_complete(DnsQuery *q) {
        int r;

        assert(q);
        assert(q->request_dns_packet);

        switch (q->state) {

        case DNS_TRANSACTION_SUCCESS: {
                _cleanup_(dns_packet_unrefp) DnsPacket *reply = NULL;

                r = dns_stub_make_reply_packet(
                                DNS_PACKET_ID(q->request_dns_packet),
                                q->answer_rcode,
                                q->question_idna,
                                q->answer,
                                !!q->request_dns_packet->opt,
                                DNS_PACKET_DO(q->request_dns_packet),
                                DNS_PACKET_DO(q->request_dns_packet) && q->answer_authenticated,
                                &reply);
                if (r < 0) {
                        log_debug_errno(r, "Failed to build reply packet: %m");
                        break;
                }

                (void) dns_stub_send(q->manager, q->request_dns_stream, q->request_dns_packet, reply);
                break;
        }

        case DNS_TRANSACTION_RCODE_FAILURE:
                (void) dns_stub_send_failure(q->manager, q->request_dns_stream, q->request_dns_packet, q->answer_rcode);
                break;

        case DNS_TRANSACTION_NOT_FOUND:
                (void) dns_stub_send_failure(q->manager, q->request_dns_stream, q->request_dns_packet, DNS_RCODE_NXDOMAIN);
                break;

        case DNS_TRANSACTION_TIMEOUT:
        case DNS_TRANSACTION_ATTEMPTS_MAX_REACHED:
                /* Propagate a timeout as a no packet, i.e. that the client also gets a timeout */
                break;

        case DNS_TRANSACTION_NO_SERVERS:
        case DNS_TRANSACTION_INVALID_REPLY:
        case DNS_TRANSACTION_ERRNO:
        case DNS_TRANSACTION_ABORTED:
        case DNS_TRANSACTION_DNSSEC_FAILED:
        case DNS_TRANSACTION_NO_TRUST_ANCHOR:
        case DNS_TRANSACTION_RR_TYPE_UNSUPPORTED:
        case DNS_TRANSACTION_NETWORK_DOWN:
                (void) dns_stub_send_failure(q->manager, q->request_dns_stream, q->request_dns_packet, DNS_RCODE_SERVFAIL);
                break;

        case DNS_TRANSACTION_NULL:
        case DNS_TRANSACTION_PENDING:
        case DNS_TRANSACTION_VALIDATING:
        default:
                assert_not_reached("Impossible state");
        }

        /* If there's a packet to write set, let's leave the stream around */
        if (q->request_dns_stream && DNS_STREAM_QUEUED(q->request_dns_stream)) {

                /* Detach the stream from our query (make it an orphan), but do not drop the reference to it. The
                 * default completion action of the stream will drop the reference. */

                dns_stub_detach_stream(q->request_dns_stream);
                q->request_dns_stream = NULL;
        }

        dns_query_free(q);
}

static int dns_stub_stream_complete(DnsStream *s, int error) {
        assert(s);

        log_debug_errno(error, "DNS TCP connection terminated, destroying query: %m");

        assert(s->query);
        dns_query_free(s->query);

        return 0;
}

static void dns_stub_process_query(Manager *m, DnsStream *s, DnsPacket *p) {
        DnsQuery *q = NULL;
        int r;

        assert(m);
        assert(p);
        assert(p->protocol == DNS_PROTOCOL_DNS);

        /* Takes ownership of the *s stream object */

        if (in_addr_is_localhost(p->family, &p->sender) <= 0 ||
            in_addr_is_localhost(p->family, &p->destination) <= 0) {
                log_error("Got packet on unexpected IP range, refusing.");
                dns_stub_send_failure(m, s, p, DNS_RCODE_SERVFAIL);
                goto fail;
        }

        r = dns_packet_extract(p);
        if (r < 0) {
                log_debug_errno(r, "Failed to extract resources from incoming packet, ignoring packet: %m");
                dns_stub_send_failure(m, s, p, DNS_RCODE_FORMERR);
                goto fail;
        }

        if (!DNS_PACKET_VERSION_SUPPORTED(p)) {
                log_debug("Got EDNS OPT field with unsupported version number.");
                dns_stub_send_failure(m, s, p, DNS_RCODE_BADVERS);
                goto fail;
        }

        if (dns_type_is_obsolete(p->question->keys[0]->type)) {
                log_debug("Got message with obsolete key type, refusing.");
                dns_stub_send_failure(m, s, p, DNS_RCODE_NOTIMP);
                goto fail;
        }

        if (dns_type_is_zone_transer(p->question->keys[0]->type)) {
                log_debug("Got request for zone transfer, refusing.");
                dns_stub_send_failure(m, s, p, DNS_RCODE_NOTIMP);
                goto fail;
        }

        if (!DNS_PACKET_RD(p))  {
                /* If the "rd" bit is off (i.e. recursion was not requested), then refuse operation */
                log_debug("Got request with recursion disabled, refusing.");
                dns_stub_send_failure(m, s, p, DNS_RCODE_REFUSED);
                goto fail;
        }

        if (DNS_PACKET_DO(p) && DNS_PACKET_CD(p)) {
                log_debug("Got request with DNSSEC CD bit set, refusing.");
                dns_stub_send_failure(m, s, p, DNS_RCODE_NOTIMP);
                goto fail;
        }

        r = dns_query_new(m, &q, p->question, p->question, 0, SD_RESOLVED_PROTOCOLS_ALL|SD_RESOLVED_NO_SEARCH|SD_RESOLVED_NO_CNAME);
        if (r < 0) {
                log_error_errno(r, "Failed to generate query object: %m");
                dns_stub_send_failure(m, s, p, DNS_RCODE_SERVFAIL);
                goto fail;
        }

        /* Request that the TTL is corrected by the cached time for this lookup, so that we return vaguely useful TTLs */
        q->clamp_ttl = true;

        q->request_dns_packet = dns_packet_ref(p);
        q->request_dns_stream = dns_stream_ref(s); /* make sure the stream stays around until we can send a reply through it */
        q->complete = dns_stub_query_complete;

        if (s) {
                s->on_packet = NULL;
                s->complete = dns_stub_stream_complete;
                s->query = q;
        }

        r = dns_query_go(q);
        if (r < 0) {
                log_error_errno(r, "Failed to start query: %m");
                dns_stub_send_failure(m, s, p, DNS_RCODE_SERVFAIL);
                goto fail;
        }

        log_info("Processing query...");
        return;

fail:
        if (s && DNS_STREAM_QUEUED(s))
                dns_stub_detach_stream(s);

        dns_query_free(q);
}

static int on_dns_stub_packet(sd_event_source *s, int fd, uint32_t revents, void *userdata) {
        _cleanup_(dns_packet_unrefp) DnsPacket *p = NULL;
        Manager *m = userdata;
        int r;

        r = manager_recv(m, fd, DNS_PROTOCOL_DNS, &p);
        if (r <= 0)
                return r;

        if (dns_packet_validate_query(p) > 0) {
                log_debug("Got DNS stub UDP query packet for id %u", DNS_PACKET_ID(p));

                dns_stub_process_query(m, NULL, p);
        } else
                log_debug("Invalid DNS stub UDP packet, ignoring.");

        return 0;
}

int manager_dns_stub_udp_fd(Manager *m) {
        static const int one = 1;

        union sockaddr_union sa = {
                .in.sin_family = AF_INET,
                .in.sin_port = htobe16(53),
                .in.sin_addr.s_addr = htobe32(INADDR_DNS_STUB),
        };

        int r;

        if (m->dns_stub_udp_fd >= 0)
                return m->dns_stub_udp_fd;

        m->dns_stub_udp_fd = socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
        if (m->dns_stub_udp_fd < 0)
                return -errno;

        r = setsockopt(m->dns_stub_udp_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = setsockopt(m->dns_stub_udp_fd, IPPROTO_IP, IP_PKTINFO, &one, sizeof(one));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = setsockopt(m->dns_stub_udp_fd, IPPROTO_IP, IP_RECVTTL, &one, sizeof(one));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        /* Make sure no traffic from outside the local host can leak to onto this socket */
        r = setsockopt(m->dns_stub_udp_fd, SOL_SOCKET, SO_BINDTODEVICE, "lo", 3);
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = bind(m->dns_stub_udp_fd, &sa.sa, sizeof(sa.in));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = sd_event_add_io(m->event, &m->dns_stub_udp_event_source, m->dns_stub_udp_fd, EPOLLIN, on_dns_stub_packet, m);
        if (r < 0)
                goto fail;

        (void) sd_event_source_set_description(m->dns_stub_udp_event_source, "dns-stub-udp");

        return m->dns_stub_udp_fd;

fail:
        m->dns_stub_udp_fd = safe_close(m->dns_stub_udp_fd);
        return r;
}

static int on_dns_stub_stream_packet(DnsStream *s) {
        assert(s);
        assert(s->read_packet);

        if (dns_packet_validate_query(s->read_packet) > 0) {
                log_debug("Got DNS stub TCP query packet for id %u", DNS_PACKET_ID(s->read_packet));

                dns_stub_process_query(s->manager, s, s->read_packet);
        } else
                log_debug("Invalid DNS stub TCP packet, ignoring.");

        /* Drop the reference to the stream. Either a query was created and added its own reference to the stream now,
         * or that didn't happen in which case we want to free the stream */
        dns_stream_unref(s);

        return 0;
}

static int on_dns_stub_stream(sd_event_source *s, int fd, uint32_t revents, void *userdata) {
        DnsStream *stream;
        Manager *m = userdata;
        int cfd, r;

        cfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK|SOCK_CLOEXEC);
        if (cfd < 0) {
                if (errno == EAGAIN || errno == EINTR)
                        return 0;

                return -errno;
        }

        r = dns_stream_new(m, &stream, DNS_PROTOCOL_DNS, cfd);
        if (r < 0) {
                safe_close(cfd);
                return r;
        }

        stream->on_packet = on_dns_stub_stream_packet;

        /* We let the reference to the stream dangling here, it will either be dropped by the default "complete" action
         * of the stream, or by our packet callback, or when the manager is shut down. */

        return 0;
}

int manager_dns_stub_tcp_fd(Manager *m) {
        static const int one = 1;

        union sockaddr_union sa = {
                .in.sin_family = AF_INET,
                .in.sin_addr.s_addr = htobe32(INADDR_DNS_STUB),
                .in.sin_port = htobe16(53),
        };

        int r;

        if (m->dns_stub_tcp_fd >= 0)
                return m->dns_stub_tcp_fd;

        m->dns_stub_tcp_fd = socket(AF_INET, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);
        if (m->dns_stub_tcp_fd < 0)
                return -errno;

        r = setsockopt(m->dns_stub_tcp_fd, IPPROTO_IP, IP_TTL, &one, sizeof(one));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = setsockopt(m->dns_stub_tcp_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = setsockopt(m->dns_stub_tcp_fd, IPPROTO_IP, IP_PKTINFO, &one, sizeof(one));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = setsockopt(m->dns_stub_tcp_fd, IPPROTO_IP, IP_RECVTTL, &one, sizeof(one));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        /* Make sure no traffic from outside the local host can leak to onto this socket */
        r = setsockopt(m->dns_stub_tcp_fd, SOL_SOCKET, SO_BINDTODEVICE, "lo", 3);
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = bind(m->dns_stub_tcp_fd, &sa.sa, sizeof(sa.in));
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = listen(m->dns_stub_tcp_fd, SOMAXCONN);
        if (r < 0) {
                r = -errno;
                goto fail;
        }

        r = sd_event_add_io(m->event, &m->dns_stub_tcp_event_source, m->dns_stub_tcp_fd, EPOLLIN, on_dns_stub_stream, m);
        if (r < 0)
                goto fail;

        (void) sd_event_source_set_description(m->dns_stub_tcp_event_source, "dns-stub-tcp");

        return m->dns_stub_tcp_fd;

fail:
        m->dns_stub_tcp_fd = safe_close(m->dns_stub_tcp_fd);
        return r;
}

int manager_dns_stub_start(Manager *m) {
        int r;

        assert(m);

        r = manager_dns_stub_udp_fd(m);
        if (r == -EADDRINUSE)
                goto eaddrinuse;
        if (r < 0)
                return r;

        r = manager_dns_stub_tcp_fd(m);
        if (r == -EADDRINUSE)
                goto eaddrinuse;
        if (r < 0)
                return r;

        return 0;

eaddrinuse:
        log_warning("Another process is already listening on 127.0.0.53:53. Turning off local DNS stub support.");
        manager_dns_stub_stop(m);

        return 0;
}

void manager_dns_stub_stop(Manager *m) {
        assert(m);

        m->dns_stub_udp_event_source = sd_event_source_unref(m->dns_stub_udp_event_source);
        m->dns_stub_tcp_event_source = sd_event_source_unref(m->dns_stub_tcp_event_source);

        m->dns_stub_udp_fd = safe_close(m->dns_stub_udp_fd);
        m->dns_stub_tcp_fd = safe_close(m->dns_stub_tcp_fd);
}
