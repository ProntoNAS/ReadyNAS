#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "des3.h"
#include "vpd.h"

//#define VPD_PATH	"/root/vpd-RNPR-01"
#define VPD_PATH	"/boot/vpd"
#define VPD_DATA_OUT	"/proc/sys/dev/boot/data_out"
#define VPD_DATA_IN	"/proc/sys/dev/boot/data_in"

#define VPD_DEVICE	"/dev/vpd"

static unsigned char vpd_buffer[512];
static unsigned char key[24];
struct vpd_data_struct vpd_data;


void read_vpd_file(void)
{
	int fd;
	fd = open(VPD_PATH, 0);
	
	lseek(fd,0,SEEK_SET);	
	read(fd, vpd_buffer,512);
	close(fd);
}

void read_vpd_data(void)
{
	int fd;
	fd = open(VPD_DATA_IN, 0);
	
	lseek(fd,0,SEEK_SET);	
	read(fd, (void*)&vpd_data,sizeof(vpd_data));
	close(fd);
}

void write_vpd_file(void)
{
	int fd;
	fd = open(VPD_PATH, 1);
	
	lseek(fd,0,SEEK_SET);	
	write(fd, vpd_buffer,512);
	close(fd);
}

#if 0
void write_vpd_data(void)
{
	int fd;
	fd = open(VPD_DATA_OUT, 1);
	
	lseek(fd,0,SEEK_SET);	
	write(fd, vpd_buffer,512);
	close(fd);
}
#else
void write_vpd_data(void)
{
        int fd;

        fd = open(VPD_DEVICE, O_RDWR|O_NONBLOCK);

        ioctl(fd, 1, &vpd_buffer);

        close(fd);
}
#endif

void init_key(void)
{
	// Assign default key values: scramble it for reverse engineering
	key[ 0] = 0x54 ^ 0x3a;
	key[ 1] = 0x72 ^ 0x3a;
	key[ 2] = 0x75 ^ 0x3a;
	key[ 3] = 0x73 ^ 0x3a;
	key[ 4] = 0x74 ^ 0x3a;
	key[ 5] = 0x47 ^ 0x3a;
	key[ 6] = 0x4f ^ 0x3a;
	key[ 7] = 0x44 ^ 0x3a;
	key[ 8] = 0x49 ^ 0xa5;
	key[ 9] = 0x6e ^ 0xa5;
	key[10] = 0x66 ^ 0xa5;
	key[11] = 0x72 ^ 0xa5;
	key[12] = 0x61 ^ 0xa5;
	key[13] = 0x6e ^ 0xa5;
	key[14] = 0x74 ^ 0xa5;
	key[15] = 0x57 ^ 0xa5;
	key[16] = 0x69 ^ 0x77;
	key[17] = 0x6c ^ 0x77;
	key[18] = 0x6c ^ 0x77;
	key[19] = 0x47 ^ 0x77;
	key[20] = 0x6f ^ 0x77;
	key[21] = 0x49 ^ 0x77;
	key[22] = 0x50 ^ 0x77;
	key[23] = 0x4f ^ 0x77;
}

void do_decrypt(void)
{
	DES3_KS k;
	int i;

	// Initialization
	gensp();		// Generate S/P box
	des3key(k, key, 1);	// Generate key schedule
	for( i = 0; i < 512/8; i++) {
		// Encypt or descript it
		des3(k, vpd_buffer+i*8);
	}
		
}
void do_encrypt(void)
{
	DES3_KS k;
	int i;

	// Initialization
	gensp();		// Generate S/P box
	des3key(k, key, 0);	// Generate key schedule
	for( i = 0; i < 512/8; i++) {
		// Encypt or descript it
		des3(k, vpd_buffer+i*8);
	}
}

int main(int argc, char *argv[])
{

	int decrypt=0;

	if(argc>1)decrypt = atoi(argv[1]);

	init_key();
	read_vpd_file();

	if(!decrypt) {
		do_encrypt();
		write_vpd_file();
	}
	else {
		do_decrypt();
		write_vpd_data();
	}


	return 0;	
}	
