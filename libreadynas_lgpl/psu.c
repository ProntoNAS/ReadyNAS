/*
    psu.c - Read PSU I2C register.
    By       Weihan Qian <jason.qian@netgear.com>
*/

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define I2C_SLAVE             0x0703
#define I2C_SMBUS             0x0720
#define I2C_SMBUS_READ        1
#define I2C_SMBUS_BYTE        1
#define I2C_SMBUS_BYTE_DATA   2
#define I2C_SMBUS_BLOCK_MAX   32

union i2c_smbus_data {
        __u8 byte;
        __u16 word;
        __u8 block[I2C_SMBUS_BLOCK_MAX+2];
};

struct i2c_smbus_ioctl_data {
        char read_write;
        __u8 command;
        int size;
        union i2c_smbus_data *data;
};

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
        union i2c_smbus_data data;
        struct i2c_smbus_ioctl_data args;

        args.read_write = I2C_SMBUS_READ;
        args.command = command;
        args.size = I2C_SMBUS_BYTE_DATA;
        args.data = &data;
        if( ioctl(file, I2C_SMBUS, &args) != 0 )
                return -1;
        else
                return 0x0FF & data.byte;
}

/* Support 2U PSU monitor */
int get_psu_power_status(int psu)
{
#if defined(__i386__) || defined(__x86_64__)
	int file = 0;
	int ret = -1, address = 0, daddress = 0xC;

	if( psu == 1 )
		address = 0x38;
	else
		address = 0x39;

	file = open("/dev/i2c-0", O_RDONLY);
	if( file < 0 )
		return -1;
	if( ioctl(file, I2C_SLAVE, address) < 0 )
		goto end_psu;
	ret = i2c_smbus_read_byte_data(file, daddress);
end_psu:
	close(file);

	return ret;
#else
	/* All other platforms have only one PSU */
	return 1;
#endif
}
