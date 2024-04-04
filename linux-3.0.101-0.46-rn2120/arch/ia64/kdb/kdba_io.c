/*
 * Kernel Debugger Architecture Dependent Console I/O handler
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 1999-2006 Silicon Graphics, Inc.  All Rights Reserved.
 */

#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/console.h>
#include <linux/ctype.h>
#include <linux/keyboard.h>
#include <linux/serial.h>
#include <linux/serial_reg.h>

#include <linux/lkdb.h>
#include <linux/kdbprivate.h>

#if defined(CONFIG_SERIAL_8250_CONSOLE) || defined(CONFIG_SERIAL_SGI_L1_CONSOLE)
#define HAVE_KDBA_SERIAL_CONSOLE
#endif

/* from include/linux/pc_keyb.h on 2.4 */
#define KBD_STATUS_REG		0x64	/* Status register (R) */
#define KBD_DATA_REG		0x60	/* Keyboard data register (R/W) */
#define KBD_CMD_SET_LEDS	0xED	/* Set keyboard leds */
#define KBD_STAT_OBF 		0x01	/* Keyboard output buffer full */
#define KBD_STAT_IBF 		0x02	/* Keyboard input buffer full */
#define KBD_STAT_MOUSE_OBF	0x20	/* Mouse output buffer full */

#ifdef	CONFIG_VT_CONSOLE
#define KDB_BLINK_LED 1
#else
#undef	KDB_BLINK_LED
#endif

/*
 * This module contains code to read characters from the keyboard or a serial
 * port.
 *
 * It is used by the kernel debugger, and is polled, not interrupt driven.
 *
 */

#ifdef	KDB_BLINK_LED
/*
 * send:  Send a byte to the keyboard controller.  Used primarily to
 * 	  alter LED settings.
 */

static void
kdb_kbdsend(unsigned char byte)
{
	int timeout;
	for (timeout = 200 * 1000; timeout && (inb(KBD_STATUS_REG) & KBD_STAT_IBF); timeout--);
	outb(byte, KBD_DATA_REG);
	udelay(40);
	for (timeout = 200 * 1000; timeout && (~inb(KBD_STATUS_REG) & KBD_STAT_OBF); timeout--);
	inb(KBD_DATA_REG);
	udelay(40);
}

static void
kdb_toggleled(int led)
{
	static int leds;

	leds ^= led;

	kdb_kbdsend(KBD_CMD_SET_LEDS);
	kdb_kbdsend((unsigned char)leds);
}
#endif	/* KDB_BLINK_LED */

#ifdef	HAVE_KDBA_SERIAL_CONSOLE

struct kdb_serial kdb_serial;
enum kdba_serial_console kdba_serial_console;
static int get_serial_char(void);

/* There must be a serial_inp_xxx() and get_serial_char_xxx() for each type
 * of console.  See enum kdba_serial_console in include/asm-$(ARCH)/kdbprivate.h.
 */

#ifdef	CONFIG_SERIAL_8250_CONSOLE

static unsigned int
serial_inp_standard(const struct kdb_serial *kdb_serial, int offset)
{
	offset <<= kdb_serial->ioreg_shift;

	switch (kdb_serial->io_type) {
	case SERIAL_IO_MEM:
		return readb((void __iomem *)(kdb_serial->iobase + offset));
		break;
	default:
		return inb(kdb_serial->iobase + offset);
		break;
	}
}

/* Check if there is a byte ready at the serial port */
static int
get_serial_char_standard(void)
{
	unsigned char ch;
	static unsigned long fifon;
	if (fifon == 0) {
		/* try to set the FIFO */
		fifon = kdb_serial.iobase +
			(UART_FCR << kdb_serial.ioreg_shift);
		switch (kdb_serial.io_type) {
		case SERIAL_IO_MEM:
			writeb((UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR |
				UART_FCR_CLEAR_XMIT), (void __iomem *)fifon);
			break;
		default:
			outb((UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR |
				UART_FCR_CLEAR_XMIT), fifon);
			break;
		}
	}

	if (kdb_serial.iobase == 0)
		return -1;

	if (serial_inp_standard(&kdb_serial, UART_LSR) & UART_LSR_DR) {
		ch = serial_inp_standard(&kdb_serial, UART_RX);
		if (ch == 0x7f)
			ch = 8;
		return ch;
	}
	return -1;
}

#else	/* !CONFIG_SERIAL_8250_CONSOLE */

#define get_serial_char_standard() -1

#endif	/* CONFIG_SERIAL_8250_CONSOLE */

#ifdef CONFIG_SERIAL_SGI_L1_CONSOLE

extern u64 master_node_bedrock_address;

/* UART registers on the Bedrock start at 0x80 */

extern int l1_serial_in_polled(void);
extern int l1_control_in_polled(int);

/* Read a byte from the L1 port.  kdb_serial is ignored */
static unsigned int
serial_inp_sgi_l1(const struct kdb_serial *kdb_serial, int offset)
{
	if (offset & 0x80) {
		int counter = 10000;
		unsigned int value;
		while ( counter-- ) {
			value = l1_serial_in_polled();
			/* Gobble up the 0's */
			if ( value )
				return(value);
		}
		return(0);
	}
	else {
		return l1_control_in_polled(offset);
	}
}

/* Check if there is a byte ready at the L1 port. */
static int
get_serial_char_sgi_l1(void)
{
	unsigned char ch;
	int status;

	if ((status = serial_inp_sgi_l1(&kdb_serial, UART_LSR)) & UART_LSR_DR) {
		ch = serial_inp_sgi_l1(&kdb_serial, UART_RX | 0x80);	/* bedrock offset */
		if (ch == 0x7f)
			ch = 8;
		return ch;
	}
	return -1;
}

#else	/* !CONFIG_SERIAL_SGI_L1_CONSOLE */

#define get_serial_char_sgi_l1() -1

#endif	/* CONFIG_SERIAL_SGI_L1_CONSOLE */

/* Select the serial console input at run time, to handle generic kernels */

static int
get_serial_char(void)
{
	switch (kdba_serial_console) {
	case KDBA_SC_NONE:
		return -1;
	case KDBA_SC_STANDARD:
		return get_serial_char_standard();
	case KDBA_SC_SGI_L1:
		return get_serial_char_sgi_l1();
	}
	/* gcc is not smart enough to realize that all paths return before here :( */
	return -1;
}

#endif /* HAVE_KDBA_SERIAL_CONSOLE */

#ifdef	CONFIG_VT_CONSOLE

static int kbd_exists;

/*
 * Check if the keyboard controller has a keypress for us.
 * Some parts (Enter Release, LED change) are still blocking polled here,
 * but hopefully they are all short.
 */
static int get_kbd_char(void)
{
	int scancode, scanstatus;
	static int shift_lock;	/* CAPS LOCK state (0-off, 1-on) */
	static int shift_key;	/* Shift next keypress */
	static int ctrl_key;
	u_short keychar;
	extern u_short plain_map[], shift_map[], ctrl_map[];

	if (LKDB_FLAG(NO_I8042) || LKDB_FLAG(NO_VT_CONSOLE) ||
	    (inb(KBD_STATUS_REG) == 0xff && inb(KBD_DATA_REG) == 0xff)) {
		kbd_exists = 0;
		return -1;
	}
	kbd_exists = 1;

	if ((inb(KBD_STATUS_REG) & KBD_STAT_OBF) == 0)
		return -1;

	/*
	 * Fetch the scancode
	 */
	scancode = inb(KBD_DATA_REG);
	scanstatus = inb(KBD_STATUS_REG);

	/*
	 * Ignore mouse events.
	 */
	if (scanstatus & KBD_STAT_MOUSE_OBF)
		return -1;

	/*
	 * Ignore release, trigger on make
	 * (except for shift keys, where we want to
	 *  keep the shift state so long as the key is
	 *  held down).
	 */

	if (((scancode&0x7f) == 0x2a) || ((scancode&0x7f) == 0x36)) {
		/*
		 * Next key may use shift table
		 */
		if ((scancode & 0x80) == 0) {
			shift_key=1;
		} else {
			shift_key=0;
		}
		return -1;
	}

	if ((scancode&0x7f) == 0x1d) {
		/*
		 * Left ctrl key
		 */
		if ((scancode & 0x80) == 0) {
			ctrl_key = 1;
		} else {
			ctrl_key = 0;
		}
		return -1;
	}

	if ((scancode & 0x80) != 0)
		return -1;

	scancode &= 0x7f;

	/*
	 * Translate scancode
	 */

	if (scancode == 0x3a) {
		/*
		 * Toggle caps lock
		 */
		shift_lock ^= 1;

#ifdef	KDB_BLINK_LED
		kdb_toggleled(0x4);
#endif
		return -1;
	}

	if (scancode == 0x0e) {
		/*
		 * Backspace
		 */
		return 8;
	}

	/* Special Key */
	switch (scancode) {
	case 0xF: /* Tab */
		return 9;
	case 0x53: /* Del */
		return 4;
	case 0x47: /* Home */
		return 1;
	case 0x4F: /* End */
		return 5;
	case 0x4B: /* Left */
		return 2;
	case 0x48: /* Up */
		return 16;
	case 0x50: /* Down */
		return 14;
	case 0x4D: /* Right */
		return 6;
	}

	if (scancode == 0xe0) {
		return -1;
	}

	/*
	 * For Japanese 86/106 keyboards
	 * 	See comment in drivers/char/pc_keyb.c.
	 * 	- Masahiro Adegawa
	 */
	if (scancode == 0x73) {
		scancode = 0x59;
	} else if (scancode == 0x7d) {
		scancode = 0x7c;
	}

	if (!shift_lock && !shift_key && !ctrl_key) {
		keychar = plain_map[scancode];
	} else if (shift_lock || shift_key) {
		keychar = shift_map[scancode];
	} else if (ctrl_key) {
		keychar = ctrl_map[scancode];
	} else {
		keychar = 0x0020;
		lkdb_printf("Unknown state/scancode (%d)\n", scancode);
	}
	keychar &= 0x0fff;
	switch (KTYP(keychar)) {
	case KT_LETTER:
	case KT_LATIN:
		if (isprint(keychar))
			break;		/* printable characters */
		/* drop through */
	case KT_SPEC:
		if (keychar == K_ENTER)
			break;
		/* drop through */
	default:
		return(-1);	/* ignore unprintables */
	}

	if ((scancode & 0x7f) == 0x1c) {
		/*
		 * enter key.  All done.  Absorb the release scancode.
		 */
		while ((inb(KBD_STATUS_REG) & KBD_STAT_OBF) == 0)
			;

		/*
		 * Fetch the scancode
		 */
		scancode = inb(KBD_DATA_REG);
		scanstatus = inb(KBD_STATUS_REG);

		while (scanstatus & KBD_STAT_MOUSE_OBF) {
			scancode = inb(KBD_DATA_REG);
			scanstatus = inb(KBD_STATUS_REG);
		}

		if (scancode != 0x9c) {
			/*
			 * Wasn't an enter-release,  why not?
			 */
			lkdb_printf("kdb: expected enter got 0x%x status 0x%x\n",
			       scancode, scanstatus);
		}

		lkdb_printf("\n");
		return 13;
	}

	return keychar & 0xff;
}
#endif	/* CONFIG_VT_CONSOLE */

#ifdef KDB_BLINK_LED

/* Leave numlock alone, setting it messes up laptop keyboards with the keypad
 * mapped over normal keys.
 */
static int kdba_blink_mask = 0x1 | 0x4;

#ifdef CONFIG_SMP
#define BOGOMIPS (local_cpu_data->loops_per_jiffy/(500000/HZ))
#else
#define BOGOMIPS (loops_per_jiffy/(500000/HZ))
#endif
static int blink_led(void)
{
	static long delay;

	if (kbd_exists == 0)
		return -1;

	if (--delay < 0) {
		if (BOGOMIPS == 0)	/* early kdb */
			delay = 150000000/1000;     /* arbitrary bogomips */
		else
			delay = 150000000/BOGOMIPS; /* Roughly 1 second when polling */
		kdb_toggleled(kdba_blink_mask);
	}
	return -1;
}
#endif

lget_char_func poll_funcs[] = {
#if defined(CONFIG_VT_CONSOLE)
	get_kbd_char,
#endif
#ifdef	HAVE_KDBA_SERIAL_CONSOLE
	get_serial_char,
#endif
#ifdef KDB_BLINK_LED
	blink_led,
#endif
	NULL
};

/* Dummy versions of kdba_local_arch_setup, kdba_local_arch_cleanup.
 * FIXME: ia64 with legacy keyboard might need the same code as i386.
 */

void kdba_local_arch_setup(void) {}
void kdba_local_arch_cleanup(void) {}
