/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * iproxy.h                                                   July 2001
 * Horms                                             horms@vergenet.net
 * Andrew Tridgell                                     tridge@samba.org
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


#ifndef _RAIDARD_PACKET_H
#define _RAIDARD_PACKET_H

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "int.h"
#include "checksum.h"

#define RAIDARD_PROTO_VERSION 1

struct packet {
	uint32 checksum;                /* Checksum of the rest of the packet */
	uint32 version;                 /* Version if raidard protocol/packet 
					 * To avoid possible problems if
					 * the packet format is later changed */
	uint32 flag;                    /* Packet flags */
	uint32 cid;                     /* id of client 
					 * Should be uniqe per client procces.
					 * The current pid wouldn't be a bad choice */
	uint32 sid;                     /* id of server. Should be unique per server. */
	uint32 length;                  /* Total length of packet */
	uint32 offset;                  /* Offset of packet. Incremented by
					 * length - PACKET_HDR_LEN for each packet */
	char buf[2048];                 /* Data */
};

#define PACKET_HDR_LEN (uint32)28

#define RAIDARD_LIST_SID ((uint32)~0)

/* Packet flags */
#define RAIDARD_NO_FLAG        0
#define RAIDARD_FIN_FLAG       1
#define RAIDARD_ACK_FLAG       2
#define RAIDARD_KEEPALIVE_FLAG 3
#define RAIDARD_LOCATE_FLAG    4
#define RAIDARD_TEST_FLAG      5
#define RAIDARD_RESET_FLAG     6
#define RAIDARD_MACCHANGED_FLAG 7
#define RAIDARD_INSTALL_FLAG   8
#define RAIDARD_STOPTIMER_FLAG 9
#define RAIDARD_FORMAT_FLAG    10

#define RAIDAR_FLAG_COMPRESS_REQ	0x80000000
#define RAIDAR_FLAG_COMPRESS_RESP	0x40000000
/**********************************************************************
 * packet_header
 * Form a packet header
 * pre: packet: pointer to packet to fill the header in of
 *      cid: Client ID for packet
 *      sid: Server ID for packet
 *      flag: Flag for packet
 *      length: Length for packet
 *      offset: Offset for packet
 *      zero_packet: Should the packet be zeroed before seeding it
 *      Note: all values for the packet will be converted from host
 *            to network byte order.
 * post: If zero_packet is non-zero
 *         Zero the packet
 *       Initialise the packet as per parameters
 * return: none
 **********************************************************************/

void packet_header(struct packet *packet, uint32 cid, uint32 sid, 
                uint32 flag, uint32 length, uint32 offset, 
                uint32 zero_packet);


/**********************************************************************
 * packet_send
 * Send a packet
 * pre: addr: Address tp send packet to
 *      packet: packet to send
 * post: Packet is sent out all network interfaces as per
 *       interface_sendto_all()
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int packet_send(struct sockaddr *addr, struct packet *packet);


/**********************************************************************
 * packet_send_keepalive
 * Send a keepalive packet packet
 * pre: addr: Address to send packet to
 *      cid: Client ID for keepalive packet
 *      sid: Server ID for keepalive packet
 * post: A keepalive packet is sent to addr with cid and sid set
 *       according to the parameters.
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

void packet_send_keepalive(struct sockaddr_in *iaddr, uint32 cid, uint32 sid);


/**********************************************************************
 * packet_send_ack
 * Send an ackpacket packet
 * pre: addr: Address to send packet to
 *      cid: Client ID for ack packet
 *      sid: Server ID for ack packet
 * post: An ack packet is sent to addr with cid and sid set
 *       according to the parameters.
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

void packet_send_ack(struct sockaddr_in *iaddr, uint32 cid, uint32 sid);


/**********************************************************************
 * packet_send_end
 * Send an end packet packet
 * pre: addr: Address to send packet to
 *      cid: Client ID for end packet
 *      sid: Server ID for end packet
 * post: An end packet is sent to addr with cid and sid set
 *       according to the parameters.
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

void packet_send_end(struct sockaddr_in *iaddr, uint32 cid, uint32 sid);


/**********************************************************************
 * packet_send_primer
 * Send a primer packet packet.
 * A primer packet is a non-control packet with zero bytes of data,
 * that is just a header. This causes the server to open a TCP connection
 * to the back-end server. If the back-end server is running a protocol
 * such as telnet where the server sends the first data across the
 * connection this will be recieved by a UDP reply from the server
 * and passed back to the end-user.
 * pre: addr: Address to send packet to
 *      cid: Client ID for primer packet
 *      sid: Server ID for primer packet
 * post: A primer packet is sent to addr with cid and sid set
 *       according to the parameters.
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

void packet_send_primer(struct sockaddr_in *iaddr, uint32 cid, uint32 sid);


/**********************************************************************
 * packet_checksum
 * Do a rolling checksum as per raidard_checksum but sckip the first word.
 * This is so the checksum itself can be inserted into the first work
 * without effecting the result of this call
 * pre: _packet: Packet to checksum
 *               _packet->length should be set to the length of the packet
 *               in network byte order.
 * post: none
 * return: Checksum of _packet, skipping the first word
 **********************************************************************/

#define packet_checksum(_packet) \
        raidard_checksum((void *)((char *)_packet + sizeof(uint32)), \
                        ntohl((_packet)->length) - sizeof(uint32))

#endif /* _RAIDARD_PACKET_H */
