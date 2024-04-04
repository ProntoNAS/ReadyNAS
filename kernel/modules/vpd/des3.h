typedef unsigned long DES_KS[16][2];	/* Single-key DES key schedule */
typedef unsigned long DES3_KS[48][2];	/* Triple-DES key schedule */

/* In deskey.c: */
void deskey(DES_KS,unsigned char *,int);
void des3key(DES3_KS,unsigned char *,int);

/* In desport.c, desborl.cas or desgnu.s: */
void des(DES_KS,unsigned char *);
/* In des3port.c, des3borl.cas or des3gnu.s: */
void des3(DES3_KS,unsigned char *);
/* Portable C code to create DES key schedules from user-provided keys
 * This doesn't have to be fast unless you're cracking keys or UNIX
 * passwords
 */

/* Key schedule-related tables from FIPS-46 */

/* permuted choice table (key) */
static unsigned char pc1[] = {
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,

	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
};

/* number left rotations of pc1 */
static unsigned char totrot[] = {
	1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28
};

/* permuted choice key (table) */
static unsigned char pc2[] = {
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

/* End of DES-defined tables */


/* bit 0 is left-most in byte */
static int bytebit[] = {
	0200,0100,040,020,010,04,02,01
};


/* Generate key schedule for encryption or decryption
 * depending on the value of "decrypt"
 */
void deskey(DES_KS k, unsigned char *key, int decrypt)
{
	unsigned char pc1m[56];		/* place to modify pc1 into */
	unsigned char pcr[56];		/* place to rotate pc1 into */
	register int i,j,l;
	int m;
	unsigned char ks[8];

	for (j=0; j<56; j++) {		/* convert pc1 to bits of key */
		l=pc1[j]-1;		/* integer bit location	 */
		m = l & 07;		/* find bit		 */
		pc1m[j]=(key[l>>3] &	/* find which key byte l is in */
			bytebit[m])	/* and which bit of that byte */
			? 1 : 0;	/* and store 1-bit result */
	}
	for (i=0; i<16; i++) {		/* key chunk for each iteration */
		memset(ks,0,sizeof(ks));	/* Clear key schedule */
		for (j=0; j<56; j++)	/* rotate pc1 the right amount */
			pcr[j] = pc1m[(l=j+totrot[decrypt? 15-i : i])<(j<28? 28 : 56) ? l: l-28];
			/* rotate left and right halves independently */
		for (j=0; j<48; j++){	/* select bits individually */
			/* check bit that goes to ks[j] */
			if (pcr[pc2[j]-1]){
				/* mask it in if it's there */
				l= j % 6;
				ks[j/6] |= bytebit[l] >> 2;
			}
		}
		/* Now convert to packed odd/even interleaved form */
		k[i][0] = ((long)ks[0] << 24)
		 | ((long)ks[2] << 16)
		 | ((long)ks[4] << 8)
		 | ((long)ks[6]);
		k[i][1] = ((long)ks[1] << 24)
		 | ((long)ks[3] << 16)
		 | ((long)ks[5] << 8)
		 | ((long)ks[7]);
	}
}

/* Generate key schedule for triple DES in E-D-E (or D-E-D) mode.
 *
 * The key argument is taken to be 24 bytes. The first 8 bytes are K1
 * for the first stage, the second 8 bytes are K2 for the middle stage
 * and the third 8 bytes are K3 for the last stage
 */
void des3key(DES_KS k, unsigned char *key, int decrypt)
{
	if(!decrypt){
		deskey(&k[0],&key[0],0);
		deskey(&k[16],&key[8],1);
		deskey(&k[32],&key[16],0);
	} else {
		deskey(&k[32],&key[0],1);
		deskey(&k[16],&key[8],0);
		deskey(&k[0],&key[16],1);
	}
}
/* Portable C version of des3() function
 * This version is very similar to that in Part V of Applied Cryptography
 * by Bruce Schneier.
 *
 * This information is in the public domain 12/15/95 P. Karn
 */
/* Combined SP lookup table, linked in
 * For best results, ensure that this is aligned on a 32-bit boundary;
 * Borland C++ 3.1 doesn't guarantee this!
 */
/* Primitive function F.
 * Input is r, subkey array in keys, output is XORed into l.
 * Each round consumes eight 6-bit subkeys, one for
 * each of the 8 S-boxes, 2 longs for each round.
 * Each long contains four 6-bit subkeys, each taking up a byte.
 * The first long contains, from high to low end, the subkeys for
 * S-boxes 1, 3, 5 & 7; the second contains the subkeys for S-boxes
 * 2, 4, 6 & 8 (using the origin-1 S-box numbering in the standard,
 * not the origin-0 numbering used elsewhere in this code)
 * See comments elsewhere about the pre-rotated values of r and Spbox.
 */
unsigned long Spbox[8][64];		/* Combined S and P boxes */
#define	F(l,r,key){\
	work = ((r >> 4) | (r << 28)) ^ key[0];\
	l ^= Spbox[6][work & 0x3f];\
	l ^= Spbox[4][(work >> 8) & 0x3f];\
	l ^= Spbox[2][(work >> 16) & 0x3f];\
	l ^= Spbox[0][(work >> 24) & 0x3f];\
	work = r ^ key[1];\
	l ^= Spbox[7][work & 0x3f];\
	l ^= Spbox[5][(work >> 8) & 0x3f];\
	l ^= Spbox[3][(work >> 16) & 0x3f];\
	l ^= Spbox[1][(work >> 24) & 0x3f];\
}
/* Encrypt or decrypt a block of data in ECB mode */
void des3(DES_KS ks, unsigned char block[8])
{
	unsigned long left,right,work;
	
	/* Read input block and place in left/right in big-endian order */
	left = ((unsigned long)block[0] << 24)
	 | ((unsigned long)block[1] << 16)
	 | ((unsigned long)block[2] << 8)
	 | (unsigned long)block[3];
	right = ((unsigned long)block[4] << 24)
	 | ((unsigned long)block[5] << 16)
	 | ((unsigned long)block[6] << 8)
	 | (unsigned long)block[7];

	/* Hoey's clever initial permutation algorithm, from Outerbridge
	 * (see Schneier p 478)	
	 *
	 * The convention here is the same as Outerbridge: rotate each
	 * register left by 1 bit, i.e., so that "left" contains permuted
	 * input bits 2, 3, 4, ... 1 and "right" contains 33, 34, 35, ... 32	
	 * (using origin-1 numbering as in the FIPS). This allows us to avoid
	 * one of the two rotates that would otherwise be required in each of
	 * the 16 rounds.
	 */
	work = ((left >> 4) ^ right) & 0x0f0f0f0f;
	right ^= work;
	left ^= work << 4;
	work = ((left >> 16) ^ right) & 0xffff;
	right ^= work;
	left ^= work << 16;
	work = ((right >> 2) ^ left) & 0x33333333;
	left ^= work;
	right ^= (work << 2);
	work = ((right >> 8) ^ left) & 0xff00ff;
	left ^= work;
	right ^= (work << 8);
	right = (right << 1) | (right >> 31);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left << 1) | (left >> 31);

	/* First key */
	F(left,right,ks[0]);
	F(right,left,ks[1]);
	F(left,right,ks[2]);
	F(right,left,ks[3]);
	F(left,right,ks[4]);
	F(right,left,ks[5]);
	F(left,right,ks[6]);
	F(right,left,ks[7]);
	F(left,right,ks[8]);
	F(right,left,ks[9]);
	F(left,right,ks[10]);
	F(right,left,ks[11]);
	F(left,right,ks[12]);
	F(right,left,ks[13]);
	F(left,right,ks[14]);
	F(right,left,ks[15]);

	/* Second key (must be created in opposite mode to first key) */
	F(right,left,ks[16]);
	F(left,right,ks[17]);
	F(right,left,ks[18]);
	F(left,right,ks[19]);
	F(right,left,ks[20]);
	F(left,right,ks[21]);
	F(right,left,ks[22]);
	F(left,right,ks[23]);
	F(right,left,ks[24]);
	F(left,right,ks[25]);
	F(right,left,ks[26]);
	F(left,right,ks[27]);
	F(right,left,ks[28]);
	F(left,right,ks[29]);
	F(right,left,ks[30]);
	F(left,right,ks[31]);

	/* Third key */
	F(left,right,ks[32]);
	F(right,left,ks[33]);
	F(left,right,ks[34]);
	F(right,left,ks[35]);
	F(left,right,ks[36]);
	F(right,left,ks[37]);
	F(left,right,ks[38]);
	F(right,left,ks[39]);
	F(left,right,ks[40]);
	F(right,left,ks[41]);
	F(left,right,ks[42]);
	F(right,left,ks[43]);
	F(left,right,ks[44]);
	F(right,left,ks[45]);
	F(left,right,ks[46]);
	F(right,left,ks[47]);

	/* Inverse permutation, also from Hoey via Outerbridge and Schneier */
	right = (right << 31) | (right >> 1);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left >> 1) | (left  << 31);
	work = ((left >> 8) ^ right) & 0xff00ff;
	right ^= work;
	left ^= work << 8;
	work = ((left >> 2) ^ right) & 0x33333333;
	right ^= work;
	left ^= work << 2;
	work = ((right >> 16) ^ left) & 0xffff;
	left ^= work;
	right ^= work << 16;
	work = ((right >> 4) ^ left) & 0x0f0f0f0f;
	left ^= work;
	right ^= work << 4;

	/* Put the block back into the user's buffer with final swap */
	block[0] = right >> 24;
	block[1] = right >> 16;
	block[2] = right >> 8;
	block[3] = right;
	block[4] = left >> 24;
	block[5] = left >> 16;
	block[6] = left >> 8;
	block[7] = left;
}
/* This program produces C source containing static initializations
 * for the combined S and P boxes in DES.
 *
 * Usage: gensp c > dessp.c (for C version)
 *        gensp a > desspa.c (for assembler versions)	
 *
 * Note! There are two different formats of the output, one for the portable
 * C version and the other for the 386/486 inline assembler version. In the
 * portable version, each entry is rotated 1 bit to the left to match the
 * format of the L and R halves in that code; this avoids one of the left
 * rotates that would otherwise be required in each round.
 *
 * In the assembler version, each entry is rotated by 2 more bits (3 total)
 * to match that version's convention of rotating the L and R halves left
 * 3 bits to avoid two shift operations that would otherwise be required
 * in each round to convert the 6-bit index into the SP array into a
 * memory offset. So be sure to run this program with the correct argument.
 *
 * August 1994, Phil Karn, KA9Q
 */


/* 32-bit permutation function P used on the output of the S-boxes */
static unsigned char p32i[] = {	
	16,  7, 20, 21,
	29, 12, 28, 17,
	 1, 15, 23, 26,
	 5, 18, 31, 10,
	 2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25
};

/* The (in)famous S-boxes */
static unsigned char sbox[8][64] = {
	/* S1 */
	{ 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13, },

	/* S2 */
	{ 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9, },

	/* S3 */
	{ 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12, },

	/* S4 */
	{  7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14, },

	/* S5 */
	{  2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3, },

	/* S6 */
	{ 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13, },

	/* S7 */
	{  4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12, },

	/* S8 */
	{ 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 }
};

int desinit(int);
void gensp(void);

void gensp(void )
{
	int i,j,k;
	int shifts;

		shifts = 1;

	desinit(0);
	k = 0;

	for(i=0;i<8;i++){
		for(j=0;j<64;j++){
			Spbox[i][j] = (Spbox[i][j] << shifts) | (Spbox[i][j] >> (32-shifts));
			if(++k == 4){
				k = 0;
			}
		}
	}
}

/* Initialize the lookup table for the combined S and P boxes
 * "mode" no longer used; only standard DES supported
 */
int desinit(int mode)
{
	unsigned char pbox[32];
	int p,i,s,j,rowcol;

	/* Compute pbox, the inverse of p32i.
	 * This is easier to work with
	 */
	for(p=0;p<32;p++){
		for(i=0;i<32;i++){
			if(p32i[i]-1 == p){
				pbox[p] = i;
				break;
			}
		}
	}
	for(s = 0; s < 8; s++){			/* For each S-box */
		for(i=0; i<64; i++){		/* For each possible input */
			Spbox[s][i] = 0;
			/* The row number is formed from the first and last
			 * bits; the column number is from the middle 4
			 */
			rowcol = (i & 32) | ((i & 1) ? 16 : 0) | ((i >> 1) & 0xf);
			for(j=0;j<4;j++){	/* For each output bit */
				if(sbox[s][rowcol] & (8 >> j)){
				 Spbox[s][i] |= 1L << (31 - pbox[4*s + j]);
				}
			}
		}
	}
	return 0;
}

