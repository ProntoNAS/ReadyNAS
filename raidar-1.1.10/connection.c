/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * connection.c                                               July 2001
 * Horms                                             horms@vergenet.net
 *
 * Code to emulate connections in UDP
 *
 * iproxy
 * UDP - TCP proxy
 * Copyright (C) 2001  Andrew Tridgell <tridge@samba.org>
 *                     Horms <horms@vergenet.net>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 **********************************************************************/

#include <time.h>

#include "util.h"
#include "raidard.h"
#include "packet.h"
#include "interface.h"
#include "connection.h"


#define RESEND_TIMEOUT (time_t)5
#define EXPIRE_TIMEOUT (time_t)15


/**********************************************************************
 * connection_dump
 * Dump information about a connection to stdout
 * pre: con: Connection to dump
 *      post: Information about the connection is printed to stdout
 * return: 0 on success
 *         -1 on error (cannot occur in current implementation)
 **********************************************************************/

int connection_dump(struct connection *con)
{
        char *tmp_str;
        time_t time_tmp;

        switch(con->status){
                case STATUS_PURGED:
                        tmp_str="purged";
                        break;
                case STATUS_CLOSED:
                        tmp_str="closed";
                        break;
                case STATUS_READY:
                        tmp_str="ready";
                        break;
                case STATUS_UNACKED:
                        tmp_str="unacked";
                        break;
                case STATUS_PENDING:
                        tmp_str="pending";
                        break;
                case STATUS_ERR:
                        tmp_str="error";
                        break;
                default:
                        tmp_str="**UNKNOWN**";
                        break;
        }
        
        printf(         "  udp_reply_addr: %s\n", 
                        inet_ntoa(con->udp_reply_addr));
        printf(         "  udp_peer_addr:  %s\n"
                        "  udp_peer_port:  %d\n"
                        "  tcp_fd:         %d\n"
                        "  cid:            0x%x\n"
                        "  sid:            0x%x\n"
                        "  in_offset:      %u\n"
                        "  out_offset:     %u\n"
                        "  status:         %s\n",
                        inet_ntoa(con->udp_peer_addr),
                        con->udp_peer_port,
                        con->tcp_fd,
                        con->cid,
                        con->sid,
                        con->in_offset,
                        con->out_offset,
                        tmp_str);
        
        time_tmp=con->expire;
        printf("  expire:         %s", ctime(&time_tmp));
        
        time_tmp=con->resend;
        printf("  resend:         %s", ctime(&time_tmp));

        return(0);
}


/**********************************************************************
 * connection_set_timeout
 * Set the timeouts for a connection 
 * The expire timeout will be set to the current time + EXPIRE_TIMEOUT
 * The resend timeout will be set to the current time + RESEND_TIMEOUT
 * pre: con: Connection to set the timeouts of
 * post: Timeouts for conenction are set
 *       nothing if con is NULL
 * return: none
 **********************************************************************/

void connection_set_timeout(struct connection *con)
{
        time_t now;

        now=time(NULL);

        con->expire = now + EXPIRE_TIMEOUT;
        con->resend = now + RESEND_TIMEOUT;
}


/**********************************************************************
 * __connection_send_packet
 * Send a packet to a given connection
 * pre: con: connection to send packet to
 *      packet: packet to send
 * post: Packet is set to connection
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

static int 
__connection_send_packet(struct connection *con, struct packet *packet)
{
        struct sockaddr_in saddr;

        bzero(&saddr, sizeof(saddr));
        saddr.sin_family=AF_INET;
        saddr.sin_port=con->udp_peer_port;
        saddr.sin_addr=con->udp_reply_addr;

        memcpy(&(con->packet), packet, sizeof(struct packet));

        if(packet_send((struct sockaddr *)&saddr, packet)){
                fprintf(stderr, "__connection_send_packet: packet_send\n");
                return(-1);
        }
        
        return(0);
}


/**********************************************************************
 * connection_resend_packet
 * Resend a packet to a given connection.
 * pre: con: connection to send packet to
 *      packet: packet to send
 * post: Packet is set to connection
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_resend_packet(struct connection *con)
{
        if(__connection_send_packet(con, &(con->packet))){
                fprintf(stderr, "connection_resend_packet: "
                                "__connection_send_packet");
                return(-1);
        }

        con->resend=time(NULL)+RESEND_TIMEOUT;

        return(0);
}


/**********************************************************************
 * connection_send_reply
 * Send a reply packet to a connection
 * pre: con: connection to send packet to
 *      packet: packet to send
 * post: packet is sent to connection
 *       timeouts for connection are set
 *       connection is marked as UNACKED
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_send_reply(struct connection *con, struct packet *packet)
{
        if(__connection_send_packet(con, packet)){
                fprintf(stderr, "connection_send_reply: "
                                "__connection_send_packet\n");
                return(-1);
        }

        connection_set_timeout(con);
        con->status=STATUS_UNACKED;

        return(0);
}


/**********************************************************************
 * connection_send_end
 * Send an END packet to a connection.
 * pre: con: connection to send packet to
 * post: END packet is sent to connection
 *       timeouts for connection are set
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_send_end(struct connection *con)
{
        struct packet packet;

        connection_packet_header(con, &packet, RAIDARD_FIN_FLAG, PACKET_HDR_LEN, 
                        1);
        if(__connection_send_packet(con, &packet)){
                fprintf(stderr, "connection_send_end: "
                                "__connection_send_packet\n");
                return(-1);
        }

        connection_set_timeout(con);

        return(0);
}


/**********************************************************************
 * connection_send_end
 * Send an ACK packet to a connection. 
 * pre: con: connection to send packet to
 * post: ACK packet is sent to connection
 *       timeouts for connection are set
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_send_ack(struct connection *con)
{
        struct packet packet;

        connection_packet_header(con, &packet, RAIDARD_ACK_FLAG, PACKET_HDR_LEN, 
                        1);
        if(__connection_send_packet(con, &packet)){
                fprintf(stderr, "connection_send_ack: "
                                "__connection_send_packet\n");
                return(-1);
        }

        connection_set_timeout(con);

        return(0);
}


/**********************************************************************
 * connection_close
 * Close a connection
 * pre: con: connection to close
 * post: END packet is sent to connection
 *       If the associated TCP socket is open it is closed
 *       status of connection is set to CLOSED
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_close(struct connection *con)
{
        int status=0;

        extern int raidard_verbose;

        if(con->status==STATUS_CLOSED || con->status==STATUS_PURGED){
                if(raidard_verbose > 1)
                        printf("Oops trying to close a closed connection\n");
                return(status);
        }

        if(connection_send_end(con)){
                fprintf(stderr, "connection_close: connection_send_end\n");
                status=-1;
        }

        if(con->tcp_fd>=0){
                close(con->tcp_fd);
                con->tcp_fd=-1;
        }
        con->status=STATUS_CLOSED;

        return(status);
}


/**********************************************************************
 * connection_purge
 * Purge a closed connectino from memory
 * pre: con: connection to purge
 * post: Memory associated with connection is zeroed, that is there
 *       is no information about the connection any more.
 *       This can only be done to a connection whose status is CLOSED.
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_purge(struct connection *con)
{
        if(con->status!=STATUS_CLOSED){
                fprintf(stderr,
                        "Oops trying to purge an unclosed connection\n");
                return(-1);
        }

        memset(con, 0, sizeof(*con));

        return(0);
}


/**********************************************************************
 * connection_timeout
 * Timeout a connection
 * pre: con: connection to timeout
 * post: If the connection's status is CLOSED
 *          purge the connection using connection_purge()
 *       Else 
 *          close the connection using connection_close()
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_timeout(struct connection *con)
{
        extern int raidard_verbose;


        if(con->status==STATUS_CLOSED){
                if(raidard_verbose > 1)
                        printf("purging connection\n");
                if(connection_purge(con)){
                        fprintf(stderr, 
                                "connection_timeout: connection_purge\n");
                        return(-1);
                }
        }
        else {
                if(raidard_verbose > 1)
                        printf("closing connection\n");
                if(connection_close(con)){
                        fprintf(stderr, 
                                "connection_timeout: connection_close\n");
                        return(-1);
                }
        }

        return(0);
}


/**********************************************************************
 * connection_read_template
 * Add the tcp socket associated with a connection to the read 
 * template if it is valid for reading
 * pre: con: Connection to add to read template
 *      read_template: Read template to ad connection to
 * post: If connection's status is PURGED
 *          do nothing
 *       Else If connection has expired
 *          reset timeouts
 *
 *       Else If connection's status is UNACKED and the resend timeout has
 *       passed
 *          resend packet
 *       Else
 *          Add connection's tcp socket to read_template
 * return: 0 socket was not added to read template
 *         1 socket was added to read template
 *         -1 on error
 **********************************************************************/

int connection_read_template(struct connection *con, fd_set *read_template)
{
        time_t now;

        if(con->status==STATUS_PURGED)
                return(0);

        now = time(NULL);

        if(now>con->expire){
                if(connection_timeout(con)) {
                        fprintf(stderr, "connection__read_template: "
                                        "connection_timeout\n");
                        return(-1);
                }
                return(0);
        }

        if (con->status==STATUS_UNACKED && now>con->resend) {
                if(connection_resend_packet(con)) {
                        fprintf(stderr, "connection_read_template: "
                                        "connection_resend_packet\n");
                        return(-1);
                }
                return(0);
        }

        if (con->status==STATUS_READY && con->tcp_fd>0) {
                FD_SET(con->tcp_fd, read_template);
        }

        return(1);
}


/**********************************************************************
 * connection_initialise
 * Initialise a connection
 * pre: con: Connection to initialise.
 *      tcp_fd: fd of tcp socket to associate with the connection
 *      udp_peer: sockaddr_in (IP address and port) of the peer
 *                (other host) communicating via UDP.
 *      udp_reply_addr: IP address to use to reply over UDP
 *                      Ignored in unicast mode, udp_peer->sin_addr is
 *                      used instead.
 *      cid: ID of client
 *      sid: ID of server
 * post: connection is intialised as per values.
 *       status of connecion is set to udp_peer_port
 *       offsets are zeroed
 *       timeouts are set as per connection_set_timeout()
 * return: none
 **********************************************************************/

void connection_initialise(struct connection *con, int tcp_fd, 
                struct sockaddr_in *udp_peer, struct in_addr *udp_reply_addr, 
                uint32 cid, uint32 sid)
{ 
        con->udp_peer_addr=udp_peer->sin_addr;
        con->udp_reply_addr=udp_peer->sin_addr;
        con->udp_peer_port=udp_peer->sin_port;
        con->status=STATUS_READY;
        con->tcp_fd=tcp_fd;
        con->in_offset=0;
        con->out_offset=0;
        con->cid=cid;
        con->sid=sid;

        connection_set_timeout(con);
}


/**********************************************************************
 * connection_command_packet
 * Proccess a command packet recieved by a connection
 * pre: con: Connection commend packet was recieved on
 *      packet: The command packet
 *      pause: Pause in nanoseconds before closing a connection, if
 *             it needs to be closed
 *             0 for no pause.
 * post: If command is ACK
 *          If the connection is unknown
 *             Error
 *          Else
 *             Reset timeouts for connection
 *             Set connection's status to READY
 *      Else If command is KEEPALIVE
 *         If the connection is unknown
 *            Error
 *         Else If the connection's status is unacked
 *            Ignore
 *         Else 
 *            Reset timeouts for connection
 *      Else (Command is FIN)
 *         Close connection
 *      Note: Closing an unknown connection is not an error. This
 *            is so if an END packet is resent by our peer and we
 *            get both of them an error will not occur.
 * return: 0 on success for non-close commands
 *         1 on success if command is close
 *         -1 on failure for non-FIN commands
 *         -2 on failure for FIN commands
 **********************************************************************/

int connection_command_packet(struct connection *con, struct packet *packet,
                uint32 pause)
{
        struct timespec req;
        struct timespec rem;
                        
        extern int raidard_verbose;

        if(ntohl(packet->flag)==RAIDARD_ACK_FLAG){
                if(con==NULL) {  
                        fprintf(stderr, "ack for unknown connection\n");
                        return(-1);
                }
                connection_set_timeout(con);
                con->status=STATUS_READY;
                if(raidard_verbose > 2)
                        printf("ack for connection\n");
                return(0);
        }

        if(ntohl(packet->flag)==RAIDARD_KEEPALIVE_FLAG){
                if(con==NULL) {  
                        fprintf(stderr, "keepalive for unknown connection\n");
                        return(-1);
                }

                if(con->status==STATUS_UNACKED) {
                        if(raidard_verbose > 2)
                                printf("ingnoring keepalive for "
                                                "unacked connection\n");
                }
                else {
                        connection_set_timeout(con);
                        if(raidard_verbose > 2)
                                printf("keepalive for connection\n");
                }
                return(0);
        }

        if(con==NULL)
                return(1);

        if(pause) {
                req.tv_sec =  pause / 1000000000;
                req.tv_nsec = pause % 1000000000;
                if(nanosleep(&req, &rem)){
                        if(errno!=EINTR) {
                                perror("connection_command_packet: nanosleep");
                                return(-2);
                        }
                }
        }

        if(connection_close(con)){
                fprintf(stderr, "connection_command_packet: "
                                "connection_close\n");
                return(-2);
        }
        return(1);
}


/**********************************************************************
 * connection_read_tcp
 * Read data from the tcp socket associated with a connection
 * pre: con: connection to read from tcp socket of
 * post: If connection's status is UNACKED
 *         do nothing
 *       Else
 *         Read data from socket
 *         If there is an error reading
 *            Close connection using connection_close()
 *         Else
 *           Send data on to UDP side of connection using
 *           connection_send_reply()
 * return: 0 on success (including if nothing is done)
 *         1 in there is an error reading from the socket
 *           in which case the connection will be closed.
 *         -1 for other errors
 **********************************************************************/

int connection_read_tcp(struct connection *con)
{
        ssize_t n;
        size_t m;
        struct packet packet;

        extern int raidard_verbose;

        bzero(&packet, sizeof(packet));

        if(con->status==STATUS_UNACKED){
                if(raidard_verbose > 3)
                        printf("Waiting for ack, not reading packet\n");
                return(0);
        }
        if((n=read(con->tcp_fd, packet.buf, sizeof(packet.buf)))<=0){
                if(connection_close(con)){
                        fprintf(stderr, "connection_read_tcp: "
                                        "connection_close\n");
                        return(-1);
                }
                return(1);
        }

        if(raidard_verbose) {
                printf("received tcp packet of size %u\n", (unsigned)n);
                if(raidard_verbose > 1)
                        printf(">%*s<\n", (int)n, packet.buf);
        }

        connection_packet_header(con, &packet, 0, n + PACKET_HDR_LEN, 0);

        m=connection_send_reply(con, &packet);
        if(m){
                fprintf(stderr, "connection_read_tcp: "
                                "connection_send_reply\n");
                connection_close(con);
                return(-1);
        }

        return(0);
}


/**********************************************************************
 * connection_write_all_tcp
 * Write to a connection
 * Write all, means keep trying writes untill all of the buffer
 * has been written or an error occurs. Usually this only
 * requires one write.
 * pre: con: Connection to write to
 *      buf: Buffer to write
 *      len: Number of bytes of buf to write
 * return: number of bytes written
 *         -1 on error
 **********************************************************************/

size_t connection_write_all_tcp(struct connection *con, void *buf, size_t len)
{
        size_t n=0;
        
        if(con->status==STATUS_PURGED || con->status==STATUS_CLOSED){
                fprintf(stderr, "connection_write_all_tcp: connection "
                                "closed\n");
                return(-1);
        }

        if(len && (n=write_all(con->tcp_fd, buf, len))<=0){
                fprintf(stderr, "connection_write_all_tcp: write_all\n");
                return(n);
        }
        else
                con->in_offset+=n;

        return(n);
}


/**********************************************************************
 * connection_get_status
 * Get the status of a connection
 * pre: con: Connection to get the status of
 * post: none
 * return: Status of the connection
 **********************************************************************/

uint32 connection_get_status(struct connection *con)
{
        return(con->status);
}


/**********************************************************************
 * connection_get_tcp_fd
 * Get the TCP socket assiated with connection
 * pre: con: Connection to get the TCP socket of
 * post: none
 * return: TCP socket of the connection
 **********************************************************************/

uint32 connection_get_tcp_fd(struct connection *con)
{
        return(con->tcp_fd);
}
 

/**********************************************************************
 * connection_get_in_offset
 * Get the offset for incoming pakcets to this connection
 * pre: con: Connection to get the offset for incoming packets
 * post: none
 * return: Offset for incoming packets
 **********************************************************************/

uint32 connection_get_in_offset(struct connection *con)
{
        return(con->in_offset);
}


/**********************************************************************
 * connection_get_out_offset
 * Get the offset for outgoing pakcets to this connection
 * pre: con: Connection to get the offset for outgoing packets
 * post: none
 * return: Offset for outgoing packets
 **********************************************************************/

uint32 connection_get_out_offset(struct connection *con)
{
        return(con->out_offset);
}


/**********************************************************************
 * connection_get_expire
 * Get the expiry time of a connection
 * pre: con: Connection to get the expiry time of
 * post: none
 * return: Expiry time for connection
 **********************************************************************/

time_t connection_get_expire(struct connection *con)
{
        return(con->expire);
}


/**********************************************************************
 * connection_get_resend
 * Get the resend time of a connection
 * pre: con: Connection to get the resend time of
 * post: none
 * return: Resend time of connection
 **********************************************************************/

time_t connection_get_resend(struct connection *con)
{
        return(con->resend);
}


/**********************************************************************
 * connection_packet_header
 * Form up the header for a packet
 * pre: con: connection to form packet for
 *      packet: where the formed packet will be stored.
 *      flag: Flag to pass to packet_header()
 *      length: length paket will have
 *      zero_packet: Passed to packet_header()
 * post: Packet is intialised using packet_header()
 *       outgoing offset of connection is incremented
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_packet_header(struct connection *con, struct packet *packet, 
                uint32 flag, uint32 length, uint32 zero_packet)
{
        if(packet==NULL){
                fprintf(stderr, "connection_packet_header: NULL packet\n");
                return(-1);
        }
        if(length<PACKET_HDR_LEN){
                fprintf(stderr, "connection_packet_header: "
                                "length<PACKET_HDR_LEN\n");
                return(-1);
        }

        packet_header(packet, con->cid, con->sid, flag, length, 
                        con->out_offset, zero_packet);

        con->out_offset+=length-PACKET_HDR_LEN;

        return(0);
}
