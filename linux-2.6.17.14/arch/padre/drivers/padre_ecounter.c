/*
 * Copyright Infrant Technologies, Inc.
 */

#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/system.h>

struct _ecounter {
  unsigned long *counter;
  struct _ecounter *next;
  void *id;
  char name[32];
};
struct _ecounter *ecounter = NULL;

struct usb_ecounter usb_ec;

//==============================================================================
// PROCs
static struct ctl_table_header *sysctl_header;

// /proc/sys/dev/ecounter/info
//    write 0 to clear all counter
//    read to get all counter
static int proc_ecounter_info (ctl_table *ctl, int write, struct file *filp,
			       void *buffer, size_t *lenp, loff_t *ppos)
{
  int ret, n, d;
  struct _ecounter *ec;

  if (write) {
    char buf[32];
    ctl->data = buf;
    ctl->maxlen = sizeof(buf);
    ret = proc_dostring(ctl, write, filp, buffer, lenp, ppos);
    n = sscanf(ctl->data, "%i", &d);
    if (n!=1)
      return -1;

    if (d)
      return -1;

    ec = ecounter;
    while (ec) {
      *(ec->counter) = 0;
      ec = ec->next;
    }
  }
  else {
    char *bigbuf, *p;
    if (! (bigbuf = kmalloc(4096, GFP_KERNEL)) )
      return -ENOMEM;

    ec = ecounter;
    p = bigbuf;
    while (ec) {
      p += sprintf(p, "%s: %lu\n", ec->name, *(ec->counter));
      ec = ec->next;
    }
    *p = '\0';
    ctl->data = bigbuf;
    ctl->maxlen = 4096;
    ret = proc_dostring(ctl, write, filp, buffer, lenp, ppos);

    kfree(bigbuf);
  }
  return ret;
}

static ctl_table ecounter_table[] = {
  { 1, "info", NULL, 0, 0644, NULL, &proc_ecounter_info, NULL, },
  { 0, }
};

static ctl_table ecounter_root[] = {
  { 1, "ecounter", NULL, 0, 0555, ecounter_table, },
  { 0, }
};

static ctl_table dev_root[] = {
  { CTL_DEV, "dev", NULL, 0, 0555, ecounter_root, },
  { 0, }
};

static struct ctl_table_header *sysctl_header;

static int __init init_sysctl(void)
{
  sysctl_header = register_sysctl_table(dev_root, 0);
  return 0;
}

static void __exit cleanup_sysctl(void)
{
  unregister_sysctl_table(sysctl_header);
}

//==============================================================================
// init/exit for module
static int __init padre_ecounter_init(void)
{
  init_sysctl();
  return 0;
}

static void __exit padre_ecounter_exit (void)
{
  cleanup_sysctl();
}

//==============================================================================
// register/unregister functions
int
register_padre_ecounter(char *name, unsigned long *counter)
{
  struct _ecounter *ec, *new_ec;

  if (strlen(name)>30)
    return -1;

  // check if name is arelady registered
  ec = ecounter;
  while (ec) {
    if (strcmp(name, ec->name)==0) {
      return -1;
    }
    if (ec->counter == counter) {
      return -1;
    }
    ec = ec->next;
  }

  // alloc memory
  if (! (new_ec = kmalloc(sizeof(struct _ecounter), GFP_KERNEL)))
    return -ENOMEM;

  // make link
  if (ecounter) {
    ec = ecounter;
    while (ec->next) {
      ec = ec->next;
    }
    ec->next = new_ec;
    ec = ec->next;
  }
  else {
    ec = ecounter = new_ec;
  }

  // init
  strcpy(ec->name, name);
  ec->counter = counter;
  *(ec->counter) = 0;
  ec->next = 0;

  return 0;
}

int
unregister_padre_ecounter(unsigned long *counter)
{
  struct _ecounter *ec, *prev_ec;

  prev_ec = 0;
  ec = ecounter;
  while (ec) {
    if (ec->counter == counter) {
      if (prev_ec) {
	prev_ec->next = ec->next;
      }
      else {
	ecounter = ec->next;
      }
      kfree(ec);
      return 0;
    }
    prev_ec = ec;
    ec = ec->next;
  }

  return -1;
}

module_init(padre_ecounter_init);
module_exit(padre_ecounter_exit);
MODULE_DESCRIPTION("Padre Error Statistic Counter");
MODULE_AUTHOR("<sales@infrant.com>");
MODULE_LICENSE("Infrant Technologies, Inc.");
