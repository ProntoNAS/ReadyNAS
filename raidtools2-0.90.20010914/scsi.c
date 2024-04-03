
/*
 * scsi.c
 *
 * Copyright (C) 2001, Red Hat, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ioctl.h>

#include <scsi/scsi.h>
#include <scsi/sg.h>
#include <scsi/scsi_ioctl.h>

#include "scsi.h"

/*
 * Function declarations that are for internal functions and therefore not
 * included in the scsires.h header file.
 */

static int scsi_get_device_id_page(scsi_sg_dev_t *);
static int scsi_get_serial_number_page(scsi_sg_dev_t *);

/*
 * Now to the functions
 */


/*
 * Function: scsi_print_device_info
 *
 * Inputs:
 *	scsi_sg_dev_t * - A pointer to a device structure.  Device
 *		information contained in this structure will be used
 *		to print out a description of the device in question.
 *
 * Outputs:
 *	None, except to stdout
 *
 * Purpose: Print out some basic information about a device for debugging
 *	use.
 */
void
scsi_print_device_info(scsi_sg_dev_t * sg_dev)
{
    if (!sg_dev)
	return;
    printf("\nName:\t\t%s\nModel:\t\t%s %s %s\nSCSI Rev:\t%d\n"
	   "Device ID:\t%d:%d:%d:%d\nBlock Size:\t%d bytes\n"
	   "Total Size:\t%d MBytes\n",
	   sg_dev->name, sg_dev->vendor, sg_dev->product,
	   sg_dev->revision, sg_dev->scsi_rev, sg_dev->scsi_dev_host,
	   sg_dev->scsi_dev_bus, sg_dev->scsi_dev_id, sg_dev->scsi_dev_lun,
	   sg_dev->block_size,
	   (sg_dev->block_size * (sg_dev->num_blocks / 1024) / 1024));
    if (sg_dev->fd != -1) {
	if (sg_dev->sg_name)
	    printf("SG Name:\t%s\n", sg_dev->sg_name);
    }
}

/*
 * Function: scsi_init_sg_device
 *
 * Inputs:
 *	int - This is a file descriptor that points to a disk device
 *		that the programmer wishes to place a SCSI reservation on.
 *	const char * - This is a pointer to a string that should be
 *		the printable name of the device (possibly as passed in
 *		on the command line)
 * Outputs:
 *	struct scsi_sg_dev* - A pointer to a newly allocated structure
 *		is returned on success and on partial success.  A NULL pointer
 *		is returned on permanent failure.  Partial success is defined
 *		as any condition that causes an otherwise init'able drive
 *		to fail to init for temporary conditions (such as we found
 *		all the needed devices and started to set up the device struct
 *		and then found out that we are getting I/O errors on most
 *		commands because someone else already has a reservation on the
 *		device).  A calling function should check the returned sg_dev
 *		struct to see if initialized == TRUE.  If it does, then the
 *		setup was complete.  If it doesn't, then the setup was only
 *		partial and the device is not yet ready for use.  The calling
 *		code should then be ready to recall this function later when
 *		it has some reason to believe the code would actually succeed.
 *		After a fully init'ed sg_dev struct is returned, the partially
 *		init'ed sg_dev struct should be free()'d by the calling code.
 *
 * Purpose: Initialize an sg device for use by this code.  Once the sg
 *	device that corresponds to disk_fd has been found, this function will
 *	issue a couple test commands to the device to test its capabilities,
 *	and will then fill out a newly allocated scsi_sg_dev struct
 *	with the information it finds.  Specifically, it will fill in the
 *	following items with the information it finds:
 *
 *	fd - the new fd that points to the sg device will be put here
 *	disk_fd - the original fd will be put here
 *	scsi_rev - the SCSI revision of the device will be filled in (we
 *		only support revision 2 or 3 devices at the moment)
 *	block_size - the block size of the device as the *device* says it is,
 *		not the block size that the block device layer reports.  It
 *		is imperative that the user not confuse this number with
 *		anything else when making reservations.  The OS may claim
 *		that a device has a 4K block size when using a 4K
 *		filesystem, but if the device is actually using 512byte blocks,
 *		then we *MUST* use that number when calculating block offsets
 *		or else the reservation will be in the wrong place.
 *	scsi_dev_{id,lun,bus,host} - these will be filled in with the values
 *		that actually belong to this device
 *
 *	If the function is unable to access the sg devices (the sg driver is
 *	not present in the kernel) or is unable to match any sg device to
 *	the passed in disk_fd device, then a NULL pointer will be returned
 *	instead of a pointer to a filled out structure.  (Other mundane
 *	errors may also result in a NULL pointer return)
 */

extern scsi_sg_dev_t *
scsi_init_sg_device(int disk_fd, const char *argv)
{
    scsi_sg_dev_t *sg_dev;
    unsigned int int_array[3], i, fd;
    scsi_send_command_t cmd;
    char buffer[128];

    if ((sg_dev = malloc(sizeof(scsi_sg_dev_t))) == NULL) {
	printf("Unable to allocate memory for sg_dev\n");
	return (NULL);
    }
    memset((void *)sg_dev, 0, sizeof(struct scsi_sg_dev));

    sg_dev->fd = -1;
    sg_dev->disk_fd = disk_fd;
    sg_dev->name = (char *)argv;
    sg_dev->initialized = FALSE;

    /*
     * Get the device ID parameters from the linux kernel for the disk
     * device (aka, /dev/sda or whatever was passed in on the command
     * line or config file).  These numbers are unique for any given
     * device.
     */
    if ((ioctl(disk_fd, SCSI_IOCTL_GET_IDLUN, &int_array[0]) == -1) ||
	(ioctl(disk_fd, SCSI_IOCTL_GET_BUS_NUMBER, &int_array[2]) == -1)) {
	perror("scsi_init_sg_device: "
	       "Unable to get ID, LUN, bus, or host of drive");
	scsi_release_sg_device(sg_dev);
	return (NULL);
    }
    sg_dev->scsi_dev_id = int_array[0] & 0xff;
    sg_dev->scsi_dev_lun = (int_array[0] >> 8) & 0xff;
    sg_dev->scsi_dev_bus = (int_array[0] >> 16) & 0xff;
    sg_dev->scsi_dev_host = int_array[2];

    /*
     * Open the /dev/sg? entries one at a time and check each of them
     * to see if the device ID values match the device ID values we have
     * on our disk device.  If they do, then we've found the /dev/sg entry
     * that matches our /dev/sd entry.  If we run out of entries, then we
     * can't go any further reliably.
     */
    i = 0;
    sprintf(buffer, "/dev/sg%d", i++);
    while ((fd = open(buffer, O_RDWR | O_NDELAY)) != -1) {
	if ((ioctl(fd, SCSI_IOCTL_GET_IDLUN, &int_array[0]) == -1) ||
	    (ioctl(fd, SCSI_IOCTL_GET_BUS_NUMBER, &int_array[2]) == -1)) {
	    perror(buffer);
	    close(fd);
	    sprintf(buffer, "/dev/sg%d", i++);
	    continue;
	}
	if ((sg_dev->scsi_dev_id == (int_array[0] & 0xff)) &&
	    (sg_dev->scsi_dev_lun == ((int_array[0] >> 8) & 0xff)) &&
	    (sg_dev->scsi_dev_bus == ((int_array[0] >> 16) & 0xff)) &&
	    (sg_dev->scsi_dev_host == int_array[2])) {
	    sg_dev->fd = fd;
	    sg_dev->sg_name = malloc(strlen(buffer) + 2);
	    if (sg_dev->sg_name != NULL)
		sprintf((void *)sg_dev->sg_name, "%s", buffer);
	    break;
	}
	close(fd);
	sprintf(buffer, "/dev/sg%d", i++);
    }
    if (sg_dev->fd == -1) {
	printf("scsi_init_sg_device: unable to find the matching "
	       "sg device\nto go with the target disk device %s\n",
	       sg_dev->name);
	scsi_release_sg_device(sg_dev);
	return (NULL);
    }

    /*
     * Be prepared to do the INQUIRY twice incase this is our first
     * command since a reset.  We might have to absorb one failed
     * INQUIRY due to a UNIT_ATTENTION sense return from the device.
     */
    do {
	cmd.outsize = 0;
	cmd.insize = 56;
	memset(cmd.buf, 0, 56);
	cmd.buf[0] = INQUIRY;
	cmd.buf[4] = 56;
	if (SEND_COMMAND(sg_dev, cmd) == -1) {
	    perror("failed on INQUIRY");
	    return (sg_dev);
	}
    } while (scsi_retryable_error(sg_dev, cmd, FALSE, DEF_WAIT));

    if (!WAS_OK(cmd)) {
	printf("%s: scsi error on INQUIRY, status byte = 0x%x\n",
	       sg_dev->name, STATUS(cmd));
	return (sg_dev);
    }

    /*
     * Make sure the device meets all of our "strict" requirements.  It
     * must be a disk drive (Direct Access medium, could actually be
     * a Direct Access tape drive, but if you start putting reservations
     * on one of those then what happens is your own fault), may not be
     * a removable medium drive, and the peripheral qualifier returned
     * from the INQUIRY command must indicate that this LUN is connected
     * to the drive.
     */
    if ((cmd.buf[0] & 0x1f) != 0) {
	printf("%s: non-disk devices are not supported\n", sg_dev->name);
	scsi_release_sg_device(sg_dev);
	return (NULL);
    }
    if ((cmd.buf[0] & 0xe0) != 0) {
	printf("%s: this device is not currently ready for use\n",
	       sg_dev->name);
	scsi_release_sg_device(sg_dev);
	return (NULL);
    }
    if (cmd.buf[1] & 0x80) {
	printf("%s: removable devices are not supported\n", sg_dev->name);
	scsi_release_sg_device(sg_dev);
	return (NULL);
    }

    /*
     * Grab the needed info from the INQUIRY results.
     */
    sg_dev->scsi_rev = cmd.buf[2] & 0x7;
    memcpy((void *)&sg_dev->vendor[0], (const void *)&cmd.buf[8], 8);
    memcpy((void *)&sg_dev->product[0], (const void *)&cmd.buf[16], 16);
    memcpy((void *)&sg_dev->revision[0], (const void *)&cmd.buf[32], 4);
    sg_dev->vendor[8] = '\0';
    sg_dev->product[16] = '\0';
    sg_dev->revision[5] = '\0';

    /*
     * First try the device ID page.  Then get the serial number.
     * One of the two should provide reasonable information.
     */
    scsi_get_device_id_page(sg_dev);
    scsi_get_serial_number_page(sg_dev);

    /*
     * Good, we've passed everything now, so mark the device as init'ed
     * and send it back to the calling function.
     */
    sg_dev->initialized = TRUE;
    return (sg_dev);
}

/*
 * Function: scsi_get_device_id_page
 *
 * Inputs:
 * 	scsi_sg_dev_t * - The sg_dev we want to get size information for
 *
 * Outputs:
 * 	int - 0 on success, 1 on any error or failure.
 *
 * Purpose:
 * 	Used to provide a unique identifier to each device.  This can then
 * 	be used to detect multipath drives on the fly.  This is the preferred
 * 	way to get the unique identifier.  However, when this isn't supported
 * 	and the serial number page is, then we use it as a backup.
 */
static int
scsi_get_device_id_page(scsi_sg_dev_t *sg_dev)
{
    scsi_send_command_t cmd;
    int i;

    /*
     * Get the Device Identification page from another INQUIRY command
     */
    do {
	cmd.outsize = 0;
	cmd.insize = 255;
	memset(cmd.buf, 0, 255);
	cmd.buf[0] = INQUIRY;
	cmd.buf[1] = 1; /* EVPD bit */
	cmd.buf[2] = 0x83; /* Device ID Page */
	cmd.buf[4] = 255;
	if (SEND_COMMAND(sg_dev, cmd) == -1) {
	    perror("failed on EVPD Page 0x83 INQUIRY");
	    return 1;
	}
    } while (scsi_retryable_error(sg_dev, cmd, FALSE, DEF_WAIT));

    /*
     * Get the product ID
     */
    if(WAS_OK(cmd)) {
	i = 4; /* location of the first descriptor */
	while(i < cmd.buf[3]) {
	    switch(cmd.buf[i + 1] & 0xf) {
		case 0:
		    sg_dev->id_nonunique = malloc(cmd.buf[i + 3] + 1);
		    if(sg_dev->id_nonunique != NULL) {
			memcpy(sg_dev->id_nonunique, &cmd.buf[i + 4],
			       cmd.buf[i + 3]);
			sg_dev->id_nonunique[cmd.buf[i + 3]] = '\0';
		    }
		    break;
		case 1:
		    sg_dev->id_vendor = malloc(cmd.buf[i + 3] + 1);
		    if(sg_dev->id_vendor != NULL) {
			memcpy(sg_dev->id_vendor, &cmd.buf[i + 4],
			       cmd.buf[i + 3]);
			sg_dev->id_vendor[cmd.buf[i + 3]] = '\0';
		    }
		    break;
		case 2:
		    memcpy((void *)&sg_dev->id_eui64[0], &cmd.buf[i + 4], 8);
		    break;
		case 3:
		    memcpy((void *)&sg_dev->id_fcph[0], &cmd.buf[i + 4], 8);
		    break;
		default:
		    break;
	    }
	    i += cmd.buf[i + 3] + 4;
	}
    } else if (WAS_SENSE(cmd) && SENSE_KEY(cmd) == ILLEGAL_REQUEST) {
	/* the device doesn't support the page requested, don't give an error
	 * since this is an optional feature */
	return 1;
    } else {
	printf("%s: scsi error on EVPD Page 0x83 INQUIRY, status byte = 0x%x\n",
	       sg_dev->name, STATUS(cmd));
	if(WAS_SENSE(cmd)) {
		printf("%s: SENSE_KEY 0x%02x, ASC 0x%02x, ASQ 0x%02x\n",
			sg_dev->name, SENSE_KEY(cmd), ASC(cmd), ASQ(cmd));
	}
	return 1;
    }
    return 0;
}

/*
 * Function: scsi_get_serial_number_page
 *
 * Inputs:
 * 	scsi_sg_dev_t * - The sg_dev we want to get size information for
 *
 * Outputs:
 * 	int - 0 on success, 1 on any error or failure.
 *
 * Purpose:
 * 	Used to provide a unique identifier to each device.  This can then
 * 	be used to detect multipath drives on the fly.  This is the
 * 	backup method we use when the device doesn't support the device
 * 	id page.  This isn't quite as reliable as the device id page because
 * 	we have no assurance from the device that the number we get will
 * 	be unique, where as several of the options from the device id page
 * 	are assured of being unique.  Once we get the serial number, which
 * 	is in ASCII form, we will concatenate it with the device Vendor
 * 	string, Model string, and Revision string to produce a rather longish,
 * 	but hopefully unique device id.  It will then be stored in the
 * 	sg_dev->id_vendor spot.
 */
static int
scsi_get_serial_number_page(scsi_sg_dev_t *sg_dev)
{
    scsi_send_command_t cmd;
    char *buffer;

    /*
     * Get the Serial Number page from another INQUIRY command
     */
    do {
	cmd.outsize = 0;
	cmd.insize = 255;
	memset(cmd.buf, 0, 255);
	cmd.buf[0] = INQUIRY;
	cmd.buf[1] = 1; /* EVPD bit */
	cmd.buf[2] = 0x80; /* Serial Number Page */
	cmd.buf[4] = 255;
	if (SEND_COMMAND(sg_dev, cmd) == -1) {
	    perror("failed on EVPD Page 0x80 INQUIRY");
	    return 1;
	}
    } while (scsi_retryable_error(sg_dev, cmd, FALSE, DEF_WAIT));

    /*
     * Get the Serial Number and make a string out of it.
     */
    if(WAS_OK(cmd)) {
	buffer = malloc(cmd.buf[3] + 1);
	if(buffer == NULL)
	    return 1;
	strncpy(&buffer[0], &cmd.buf[4], cmd.buf[3]);
	buffer[cmd.buf[3]] = '\0';
	sg_dev->serial_number = buffer;
    } else if (WAS_SENSE(cmd) && SENSE_KEY(cmd) == ILLEGAL_REQUEST) {
	/* the device doesn't support the page requested, don't give an error
	 * since this is an optional feature */
	return 1;
    } else {
	printf("%s: scsi error on EVPD Page 0x80 INQUIRY, status byte = 0x%x\n",
	       sg_dev->name, STATUS(cmd));
	if(WAS_SENSE(cmd)) {
		printf("%s: SENSE_KEY 0x%02x, ASC 0x%02x, ASQ 0x%02x\n",
			sg_dev->name, SENSE_KEY(cmd), ASC(cmd), ASQ(cmd));
	}
	return 1;
    }
    return 0;
}

/*
 * Function: scsi_init_device_size
 *
 * Inputs:
 * 	scsi_sg_dev_t * - The sg_dev we want to get size information for
 *
 * Outputs:
 * 	int - 0 on success, 1 if the drive is currently reserved, 2 on any
 * 		other failure.
 * 	scsi_sg_dev_t * - the size information will be placed into the
 * 		scsi_sg_dev struct on success.
 *
 * Purpose:
 * 	This was part of scsi_init_sg_device(), but in order to make that
 * 	function more graceful in the event that the device is currently
 * 	reserved by another host at the time we are trying to initialize it,
 * 	we separated this function out.  This is only really needed if you
 * 	plan to use extent based reservations on less than the whole drive
 * 	anyway, so it can be skipped entirely if you know that you will only
 * 	need to reserve whole drives.
 */

int
scsi_init_device_size(scsi_sg_dev_t * sg_dev)
{
    scsi_send_command_t cmd;

    /*
     * Not everything we needed is part of an INQUIRY return.  Next, we
     * want to establish the block size for any extent based reservations.
     * Try to get block_size and num_blocks first by using a READ_CAPACITY,
     * but since that command isn't mandatory in the spec, fall back to
     * a MODE_SENSE and general block descriptors if it isn't available.
     * If both methods give permanent failures, then assume 512 bytes per
     * block and ignore the num_blocks element.
     */
    do {
	cmd.outsize = 0;
	cmd.insize = 16;
	memset(cmd.buf, 0, 16);
	cmd.buf[0] = READ_CAPACITY;
	if (SEND_COMMAND(sg_dev, cmd) == -1) {
	    perror("failed on READ_CAPACITY");
	    return (2);
	}
    } while (scsi_retryable_error(sg_dev, cmd, FALSE, DEF_WAIT));

    if (WAS_SENSE(cmd) && (SENSE_KEY(cmd) == ILLEGAL_REQUEST)) {
	do {
	    cmd.outsize = 0;
	    cmd.insize = 255;
	    memset(cmd.buf, 0, 255);
	    cmd.buf[0] = MODE_SENSE;
	    cmd.buf[4] = 255;
	    if (SEND_COMMAND(sg_dev, cmd) == -1) {
		perror("failed on MODE_SENSE");
		return (2);
	    }
	} while (scsi_retryable_error(sg_dev, cmd, FALSE, DEF_WAIT));

	if (RES_CONFLICT(cmd)) {
	    printf("%s: device is currently reserved.\n", sg_dev->name);
	    return (1);
	} else if (!WAS_OK(cmd) || (cmd.buf[3] != 8)) {
	    printf("%s: both MODE_SENSE and READ_CAPACITY failed "
		   "to return a block size\n", sg_dev->name);
	    printf("\tassuming 512 bytes per block\n");
	    sg_dev->block_size = 512;
	    sg_dev->num_blocks = 0;
	    return (2);
	} else {
	    sg_dev->num_blocks =
		((cmd.buf[4] << 24) | (cmd.buf[5] << 16) | (cmd.buf[6] << 8) |
		 cmd.buf[7]);
	    sg_dev->block_size =
		((cmd.buf[9] << 16) | (cmd.buf[10] << 8) | cmd.buf[11]);
	}
    } else if (WAS_SENSE(cmd)) {
	printf("%s: failure on READ_CAPACITY, SENSE_KEY 0x%02x\n",
	       sg_dev->name, SENSE_KEY(cmd));
	return (2);
    } else if (RES_CONFLICT(cmd)) {
	printf("%s: the device is currently reserved.\n", sg_dev->name);
	return (1);
    } else if (!WAS_OK(cmd)) {
	printf("%s: failure on READ_CAPACITY, status byte 0x%02x\n",
	       sg_dev->name, STATUS(cmd));
	return (2);
    } else {
	sg_dev->num_blocks =
	    1 +
	    ((cmd.buf[0] << 24) | (cmd.buf[1] << 16) | (cmd.buf[2] << 8) |
	     cmd.buf[3]);
	sg_dev->block_size =
	    (cmd.buf[4] << 24) | (cmd.buf[5] << 16) | (cmd.
						       buf[6] << 8) |
	    cmd.buf[7];
    }
    return (0);
}


/*
 * Function: scsi_release_sg_device
 *
 * Inputs:
 * 	scsi_sg_dev_t * - Pointer to the sg_dev structure to be released.
 *
 * Outputs:
 * 	int - On success, 0.  On failure, 1.
 *
 * Purpose:
 * 	This routine will free all current reservations on a device that
 * 	are listed in the reservations list.  It will then free() any memory
 * 	that has been malloc()'ed for the sg_name item, close the file
 * 	descriptor that points to the sg device entry, and then free() the
 * 	sg_dev struct itself.  The calling program is responsible for
 * 	closing the file descriptor to the block device (aka, sg_dev->disk_fd)
 * 	itself.  Since scsi_init_sg_device() doesn't open that file
 * 	descriptor, but instead simply records the one that was passed in to 
 * 	it, it was deemed inconsistent for this function to go around closing
 * 	file descriptors that the init function didn't open().  The calling
 * 	function is also responsible for calling free() on the sg_dev->name
 * 	buffer that was passed to scsi_init_sg_device() if appropriate.
 */

int
scsi_release_sg_device(scsi_sg_dev_t * sg_dev)
{
    if (sg_dev == NULL)
	return (0);
    if (sg_dev->sg_name)
	free((void *)sg_dev->sg_name);
    if (sg_dev->id_nonunique)
	free((void *)sg_dev->id_nonunique);
    if (sg_dev->id_vendor)
	free((void *)sg_dev->id_vendor);
    if (sg_dev->serial_number)
	free((void *)sg_dev->serial_number);
    close(sg_dev->fd);
    free(sg_dev);
    return (0);
}


/*
 * Function: scsi_wait_device_ready
 *
 * Inputs:
 * 	scsi_sg_dev_t * - The device to wait on
 * 	int - Amount of time to wait (in seconds) before giving up.
 *
 * Outputs:
 * 	int - 0 if device is ready, 1 if we timed out, -1 on error.
 *
 * Purpose:
 * 	This function will spin in a loop waiting for a device to become
 * 	ready.  It will return when either the device reports that it is
 * 	ready for use or the timeout has passed, whichever happens first.
 *
 */

int
scsi_wait_device_ready(scsi_sg_dev_t * sg_dev, int timeout)
{
    scsi_send_command_t cmd;
    int i;

    if (timeout < 0 || timeout > 60) {
	printf("%s: wait timeout out of range (%d)\n", sg_dev->name, timeout);
	printf("Allowed timeout is between 0 and 60 seconds.\n");
	return (-1);
    }
    i = 0;
    do {
	if (i)
	    usleep(100);
	cmd.outsize = 0;
	cmd.insize = 16;
	memset(cmd.buf, 0, 16);
	cmd.buf[0] = TEST_UNIT_READY;
	cmd.buf[1] = sg_dev->scsi_dev_lun << 5;
	if (SEND_COMMAND(sg_dev, cmd) == -1) {
	    perror("failure sending TEST_UNIT_READY");
	    return (-1);
	}
	if (!WAS_SENSE(cmd)) {
	    break;
	}
    } while ((SENSE_KEY(cmd) != NO_SENSE) && (++i < (timeout * 10)));

    if (STATUS(cmd) == GOOD || SENSE_KEY(cmd) == NO_SENSE)
	return (0);
    else
	return (1);

}
