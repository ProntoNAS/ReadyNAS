/* -*- c-file-style: "linux"; -*- */
/**********************************************************************
 * packet.c                                                   July 2001
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


#include "packet.h"
#include "interface.h"


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
                uint32 zero_packet)
{
        if(zero_packet)
		memset(packet, '\0', sizeof(struct packet));

        packet->version = htonl(RAIDARD_PROTO_VERSION);
        packet->flag = htonl(flag);
        packet->cid = htonl(cid);
        packet->sid = htonl(sid);
        packet->length = htonl(length);
        packet->offset = htonl(offset);
}


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

int packet_send(struct sockaddr *addr, struct packet *packet)
{
	extern int raidard_verbose;

	if(raidard_verbose)
		printf("Sending packet with sid=0x%x cid=0x%x, flags=0x%x\n",
			ntohl(packet->sid), ntohl(packet->cid),
			ntohl(packet->flag));

	packet->checksum = htonl(packet_checksum(packet));

	if (interface_sendto_all(packet, ntohl(packet->length),
			0, addr, sizeof(struct sockaddr))<0) {
		perror("packet_send: interface_sendto_all");
		return(-1);
	}

	return(0);
}


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

void packet_send_keepalive(struct sockaddr_in *iaddr, uint32 cid, uint32 sid)
{
	struct packet packet;

        extern int raidard_verbose;

        packet_header(&packet, cid, sid, RAIDARD_KEEPALIVE_FLAG, 
                        PACKET_HDR_LEN, 0, 1);
        packet.checksum=htonl(packet_checksum(&packet));
	if(interface_sendto_all(&packet, PACKET_HDR_LEN, 0,
	          (struct sockaddr *)iaddr, sizeof(*iaddr))<=0){
                perror("packet_send_ack: interface_send_to_all");
                exit(-1);
        }
	if (raidard_verbose)
		printf("Sent keepalive with sid=0x%x cid=0x%x\n", 
                                htonl(packet.sid), htonl(packet.cid));
}


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

void packet_send_ack(struct sockaddr_in *iaddr, uint32 cid, uint32 sid)
{
	struct packet packet;

        extern int raidard_verbose;

        packet_header(&packet, cid, sid, RAIDARD_ACK_FLAG, PACKET_HDR_LEN, 0, 1);
        packet.checksum=htonl(packet_checksum(&packet));
	if(interface_sendto_all(&packet, PACKET_HDR_LEN, 0,
	          (struct sockaddr *)iaddr, sizeof(*iaddr))<=0){
                perror("packet_send_ack: interface_send_to_all");
                exit(-1);
        }
	if (raidard_verbose)
		printf("sent ack with sid=0x%x cid=0x%x\n", htonl(packet.sid), 
                                htonl(packet.cid));
}

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

void packet_send_end(struct sockaddr_in *iaddr, uint32 cid, uint32 sid)
{
	struct packet packet;

        extern int raidard_verbose;

        packet_header(&packet, cid, sid, RAIDARD_FIN_FLAG, PACKET_HDR_LEN, 0, 1);
        packet.checksum=htonl(packet_checksum(&packet));
	if(interface_sendto_all(&packet, PACKET_HDR_LEN, 0,
	          (struct sockaddr *)iaddr, sizeof(*iaddr))<=0){
                perror("packet_send_ack: interface_send_to_all");
                exit(-1);
        }

	if (raidard_verbose)
		printf("sent end with sid=0x%x cid=0x%x\n", htonl(packet.sid), 
                                htonl(packet.cid));
}


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

void packet_send_primer(struct sockaddr_in *iaddr, uint32 cid, uint32 sid)
{
	struct packet packet;
	extern int raidard_verbose;

	packet_header(&packet, cid, sid, RAIDAR_FLAG_COMPRESS_REQ, PACKET_HDR_LEN, 0, 1);
	packet.checksum = htonl(packet_checksum(&packet));
	if (interface_sendto_all(&packet, PACKET_HDR_LEN, 0,
			(struct sockaddr *)iaddr, sizeof(*iaddr)) <= 0) {
		perror("packet_send_primer: interface_sendto_all");
		exit(-1);
	}
	if (raidard_verbose)
		printf("sent primer with sid=0x%x cid=0x%x\n flags=0x%x\n",
			htonl(packet.sid), htonl(packet.cid), htonl(packet.flag));
}
