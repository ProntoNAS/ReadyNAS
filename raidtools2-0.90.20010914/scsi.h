
/*
 * scsi.h
 *
 * Copyright (C) 2001 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * 08/27/01 - Initial version snagged by Doug Ledford from the scsires
 * 		package.
 */


/*
 * A few items missing from the scsi.h header
 */

#ifndef SCSI_TRY_RESET_DEVICE
#define SCSI_TRY_RESET_DEVICE 1
#define SCSI_TRY_RESET_BUS    2
#define SCSI_TRY_RESET_HOST   3
#endif

#define DEF_WAIT	10	/* how long we wait, in seconds, for a drive to
				 * become ready by default */

typedef enum {
    FALSE = 0,
    TRUE = 1
} bool;

struct scsi_sg_dev {
    int fd;
    int disk_fd;
    int scsi_rev;
    bool initialized;
    unsigned int block_size;
    unsigned int num_blocks;
    unsigned int scsi_dev_id;
    unsigned int scsi_dev_lun;
    unsigned int scsi_dev_bus;
    unsigned int scsi_dev_host;
    char *name;
    char *sg_name;
    char vendor[9];
    char product[17];
    char revision[5];
    char *serial_number;
    char *id_nonunique;
    char *id_vendor;
    unsigned int id_eui64[2];
    unsigned int id_fcph[2];
};

typedef struct scsi_sg_dev scsi_sg_dev_t;

/*
 * This should be in scsi.h to help make things easier, but it isn't.  So,
 * this is a makeshift struct to use to send as the argument to a
 * SCSI_IOCTL_SEND_COMMAND ioctl.
 */
struct scsi_send_command {
    int outsize;		/* bytes to be written to the bus */
    int insize;			/* bytes to be read from the bus */
    unsigned char buf[512];	/* the char array we put the
				 * data into, this may need to
				 * be larger in specific cases
				 */
    int result;			/* where we'll store the ioctl
				 * return value for testing
				 */
};

typedef struct scsi_send_command scsi_send_command_t;

/*
 * Convenient macros
 */

#define WAS_OK(x) (((x).result & 0x3f) == 0)
#define STATUS(x) (((x).result >> 1) & 0x1f)
#define WAS_SENSE(x) (((x).result & 0x3f) == 0x02)
#define RES_CONFLICT(x) (((x).result & 0x3f) == 0x18)
#define SENSE_KEY(x) ((x).buf[2] & 0x07)
#define ASC(x) ((x).buf[12])
#define ASQ(x) ((x).buf[13])

#define SEND_COMMAND(x,y) \
	((y).result = ioctl((x)->fd,SCSI_IOCTL_SEND_COMMAND,&(y)))

/*
 * Now the function declarations
 */

extern scsi_sg_dev_t *scsi_init_sg_device(int, const char *);
extern int scsi_init_device_size(scsi_sg_dev_t *);
extern int scsi_release_sg_device(scsi_sg_dev_t *);
extern void scsi_print_device_info(scsi_sg_dev_t *);
extern int scsi_wait_device_ready(scsi_sg_dev_t *, int);

/*
 * The inline functions that we provide
 */

/*
 * Function: scsi_reset_dev (inline)
 *
 * Inputs:
 *      scsi_sg_dev_t * - The device which we want reset.
 *      int - The way we want the reset performed.
 *
 * Outputs:
 *      int - 0 on success, non-zero on failure.
 *
 * Purpose:
 *      Reset the device.  We have several options for how to perform the
 *      reset.  The SCSI_TRY_RESET_* defines are the possible reset
 *      types.
 */

static inline int
scsi_reset_dev(scsi_sg_dev_t * sg_dev, int reset_type)
{
    return (ioctl(sg_dev->fd, SG_SCSI_RESET, &reset_type));
}


/*
 * Function: scsi_retryable_error (inline)
 *
 * Inputs:
 *      scsi_sg_dev_t * - The device which we sent the command to
 *      scsi_send_command_t - The command we sent
 *      bool - if there is a reservation conflict, should we force a reset
 *      	and then retry?
 *      int - the timeout to use for waiting on a drive to become ready
 *
 * Outputs:
 *      int - 1 if we should retry the error, 0 if the errors are considered
 *      	fatal, or if there were no errors and the command is complete
 *
 * Purpose:
 * 	This is just a shorthand function to make the code easier to read.
 * 	Almost all commands sent by this program will use this code to
 * 	determine if the command should be retried.  Putting it here
 * 	makes the rest of the code *much* easier to read.
 */

static inline int
scsi_retryable_error(scsi_sg_dev_t * sg_dev, scsi_send_command_t cmd,
			bool force, int timeout)
{
    int result;

    if (WAS_SENSE(cmd) && (SENSE_KEY(cmd) == UNIT_ATTENTION)) {
	return (1);
    } else if (WAS_SENSE(cmd) && (SENSE_KEY(cmd) == NOT_READY)) {
	result = scsi_wait_device_ready(sg_dev, timeout);
	if (result == 0) {
	    return (1);
	} else if (result == 1) {
	    printf("%s: device not ready error.\n", sg_dev->name);
	    return (0);
	} else {
	    return (0);
	}
    } else if (RES_CONFLICT(cmd) && (force == TRUE)) {
	result = scsi_reset_dev(sg_dev, SCSI_TRY_RESET_BUS);
	if (result) {
	    printf("%s: reset error.\n", sg_dev->name);
	    return (0);
	}
	result = scsi_wait_device_ready(sg_dev, timeout);
	if (result == 0) {
	    return (1);
	} else if (result == 1) {
	    printf("%s: device not ready error.\n", sg_dev->name);
	    return (0);
	} else {
	    return (0);
	}
    }
    return (0);
}
