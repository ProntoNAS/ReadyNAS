unsigned char *base64(unsigned char *instr)
{
	static unsigned char out[256];
	unsigned char *p = out;
	unsigned char ch = 0;
	int count = 0;
	unsigned char tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


/*
	ABC -> 414243 -> 0100 0001  0100 0010  0100 0011
				->   010000 010100 001001 000011
					 10 14 09 03
*/


	while (*instr && (p-out) < sizeof(out)-1) {
		switch (count) {
		case 0:
			ch   = (*instr >> 2)&0x3f;
			break;
		case 1:
			ch   = (*instr++ << 4)&0x3f;
			ch  |= (*instr >> 4)&0x3f;
			break;
		case 2:
			ch   = (*instr++ << 2)&0x3f;
			ch  |= (*instr >> 6)&0x3f;
			break;
		case 3:
			ch   = (*instr++)&0x3f;
			break;
		}
		count++;
		count %= 4;
		*p++ = tbl[ch];
	}
	count = (4-count)%4;
	while (count-- > 0 && (p-out) < sizeof(out)-1) {
		*p++ = '=';
	}
	*p = '\0';
	return out;
}
