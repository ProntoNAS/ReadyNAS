/*
 * $Id: crc.h 3118 2001-11-19 20:45:35Z fredrikt $
 */
 
#ifndef CRC_H
#define CRC_H

unsigned short canon_psa50_gen_crc(const unsigned char *pkt, int len);
int canon_psa50_chk_crc(const unsigned char *pkt, int len, unsigned short crc);

#endif
