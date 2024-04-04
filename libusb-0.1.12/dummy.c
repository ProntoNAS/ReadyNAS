/*
 * dummy USB support
 *
 * Derived from BSD version by Samuel Thibault
 *
 * This library is covered by the LGPL, read LICENSE for details.
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include "usbi.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

int usb_os_open(usb_dev_handle *dev)
{
  return 0;
}

int usb_os_close(usb_dev_handle *dev)
{
  return 0;
}

int usb_set_configuration(usb_dev_handle *dev, int configuration)
{
  return 0;
}

int usb_claim_interface(usb_dev_handle *dev, int interface)
{
  return 0;
}

int usb_release_interface(usb_dev_handle *dev, int interface)
{
  return 0;
}

int usb_set_altinterface(usb_dev_handle *dev, int alternate)
{
  return 0;
}

int usb_bulk_write(usb_dev_handle *dev, int ep, const char *bytes, int size,
                   int timeout)
{
  return -ENOSYS;
}

int usb_bulk_read(usb_dev_handle *dev, int ep, char *bytes, int size,
                  int timeout)
{
  return -ENOSYS;
}

int usb_interrupt_write(usb_dev_handle *dev, int ep, const char *bytes, int size,
                        int timeout)
{
  return -ENOSYS;
}

int usb_interrupt_read(usb_dev_handle *dev, int ep, char *bytes, int size,
                       int timeout)
{
  return -ENOSYS;
}

int usb_control_msg(usb_dev_handle *dev, int requesttype, int request,
                     int value, int index, char *bytes, int size, int timeout)
{
  return -ENOSYS;
}

int usb_os_find_busses(struct usb_bus **busses)
{
  *busses = NULL;
  return 0;
}

int usb_os_find_devices(struct usb_bus *bus, struct usb_device **devices)
{
  *devices = NULL;
  return 0;
}

int usb_os_determine_children(struct usb_bus *bus)
{
  return -ENOSYS;
}

void usb_os_init(void)
{
}

int usb_resetep(usb_dev_handle *dev, unsigned int ep)
{
  return -ENOSYS;
}

int usb_clear_halt(usb_dev_handle *dev, unsigned int ep)
{
  return -ENOSYS;
}

int usb_reset(usb_dev_handle *dev)
{
  return -ENOSYS;
}

