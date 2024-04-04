/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * connection.h                                               July 2001
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

#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "raidard.h"
#include "packet.h"


struct connection {
        struct in_addr udp_reply_addr;
        struct in_addr udp_peer_addr;
        uint32 udp_peer_port;
        int tcp_fd;
        uint32 cid;
        uint32 sid;
        uint32 in_offset;
        uint32 out_offset;
        uint32 status;
        time_t expire;
        time_t resend;
        struct packet packet;
};

#define STATUS_PURGED 0       /* Connection has no state */
#define STATUS_CLOSED 1       /* Connection is closed. State is kept
                               * for a while in case any stray packets come
                               * in for the connection */
#define STATUS_READY 2        /* Connection is ready to recieve data
                               * on either its UDP or TCP socket */
#define STATUS_UNACKED 3      /* Sent data to UDP socket and waiting for ACK.
                               * Data will not be read from the TCP socket
                               * while in this state. */
#define STATUS_PENDING 4      /* Recieved data from UDP socket but hasn't
                               * been processed yet. Data will not be read 
                               * from the TCP state. */
#define STATUS_ERR ((uint32)~0)



/**********************************************************************
 * connection_dump
 * Dump information about a connection to stdout
 * pre: con: Connection to dump
 *      post: Information about the connection is printed to stdout
 * return: 0 on success
 *         -1 on error (cannot occur in current implementation)
 **********************************************************************/

int connection_dump(struct connection *con);


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

void connection_set_timeout(struct connection *con);


/**********************************************************************
 * connection_resend_packet
 * Resend a packet to a given connection.
 * pre: con: connection to send packet to
 *      packet: packet to send
 * post: Packet is set to connection
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_resend_packet(struct connection *con);


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

int connection_send_reply(struct connection *con, struct packet *packet);


/**********************************************************************
 * connection_send_end
 * Send an END packet to a connection.
 * pre: con: connection to send packet to
 * post: END packet is sent to connection
 *       timeouts for connection are set
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_send_end(struct connection *con);


/**********************************************************************
 * connection_send_end
 * Send an ACK packet to a connection. 
 * pre: con: connection to send packet to
 * post: ACK packet is sent to connection
 *       timeouts for connection are set
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int connection_send_ack(struct connection *con);


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

int connection_close(struct connection *con);


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

int connection_purge(struct connection *con);


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

int connection_timeout(struct connection *con);


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

int connection_read_template(struct connection *con, fd_set *read_template);


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
                uint32 cid, uint32 sid);


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
                uint32 pause);


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

int connection_read_tcp(struct connection *con);


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

size_t connection_write_all_tcp(struct connection *con, void *buf, size_t len);


/**********************************************************************
 * connection_get_status
 * Get the status of a connection
 * pre: con: Connection to get the status of
 * post: none
 * return: Status of the connection
 **********************************************************************/

uint32 connection_get_status(struct connection *con);


/**********************************************************************
 * connection_get_tcp_fd
 * Get the TCP socket assiated with connection
 * pre: con: Connection to get the TCP socket of
 * post: none
 * return: TCP socket of the connection
 **********************************************************************/

uint32 connection_get_tcp_fd(struct connection *con);
 

/**********************************************************************
 * connection_get_in_offset
 * Get the offset for incoming pakcets to this connection
 * pre: con: Connection to get the offset for incoming packets
 * post: none
 * return: Offset for incoming packets
 **********************************************************************/

uint32 connection_get_in_offset(struct connection *con);


/**********************************************************************
 * connection_get_out_offset
 * Get the offset for outgoing pakcets to this connection
 * pre: con: Connection to get the offset for outgoing packets
 * post: none
 * return: Offset for outgoing packets
 **********************************************************************/

uint32 connection_get_out_offset(struct connection *con);


/**********************************************************************
 * connection_get_expire
 * Get the expiry time of a connection
 * pre: con: Connection to get the expiry time of
 * post: none
 * return: Expiry time for connection
 **********************************************************************/

time_t connection_get_expire(struct connection *con);


/**********************************************************************
 * connection_get_resend
 * Get the resend time of a connection
 * pre: con: Connection to get the resend time of
 * post: none
 * return: Resend time of connection
 **********************************************************************/

time_t connection_get_resend(struct connection *con);


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
                uint32 flag, uint32 length, uint32 zero_packet);

#endif /* _CONNECTION_H */
