#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>

#include "des3.h"

static unsigned char vpd_buffer[512];
static unsigned char key[24];

void read_vpd_file(char *file)
{
	int fd;
	fd = open(file, O_RDONLY);
	
	if( fd < 0 ) {
		fprintf(stderr, "ERROR: Could not open input file [%s]\n", file);
		exit(2);
	}

	lseek(fd, 0, SEEK_SET);	
	read(fd, vpd_buffer, sizeof(vpd_buffer));
	close(fd);
}

void write_vpd_file(char *file)
{
	int fd;
	fd = open(file, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
	
	if( fd < 0 ) {
		fprintf(stderr, "ERROR: Could not open output file [%s]\n", file);
		exit(2);
	}

	lseek(fd, 0, SEEK_SET);	
	write(fd, vpd_buffer, 512);
	close(fd);
}

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

void do_decrypt()
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

void usage(char *name)
{
	printf("Usage: %s [-ed] input_file output_file\n", name);
	printf("  -e	encrypt\n");
	printf("  -d	decrypt\n");
}

int main(int argc, char **argv)
{
	int encrypt = 0;
	int decrypt = 0;
	int c;
	char infile[512];
	char outfile[512];

	opterr = 0;

	while ((c = getopt (argc, argv, "ed")) != -1)
	switch (c)
	{
		case 'e':
			encrypt = 1;
			break;
		case 'd':
			decrypt = 1;
			break;
		case '?':
			if ( isprint(optopt) )
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
		default:
			abort ();
	}

	if( !encrypt && !decrypt ) {
		fprintf(stderr, "No encrypt or decrypt option specified!\n");
		usage(argv[0]);
		return(1);
	}
	else if( encrypt && decrypt ) {
		fprintf(stderr, "Cannot specify both encrypt or decrypt options!\n");
		usage(argv[0]);
		return(1);
	}
	else if (optind != argc - 2) {
		usage(argv[0]);
		return(1);
	}

	strncpy(infile, argv[optind], sizeof(infile));
	strncpy(outfile, argv[optind+1], sizeof(outfile));

	init_key();
	read_vpd_file(infile);

	if(encrypt)
		do_encrypt();
	else
		do_decrypt();

	write_vpd_file(outfile);

	return 0;	
}	
