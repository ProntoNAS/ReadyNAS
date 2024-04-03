/*
 *  linux/arch/padre/drivers/padre_serial.c
 *
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *
 *  Copyright (C) 2004 Bin Zhang
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * A note about mapbase / membase
 *
 *  mapbase is the physical address of the IO port.
 *  membase is an 'ioremapped' cookie.
 */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/tty.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/serial.h>
#include <linux/delay.h>
#include <linux/device.h>

#include <asm/io.h>
#include <asm/irq.h>

#if defined(CONFIG_PADRE_SERIAL_CONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
#define SUPPORT_SYSRQ
#endif

#include <linux/serial_core.h>

#define UART_TX		0x0
#define UART_RX		0x0
#define UART_LSR	0x4
#define UART_LCR	0x8
#define UART_DIV	0xc
#define UART_IER	UART_LCR

#define UART_LSR_TEMT   PADRE_UART0_STAT_TH
#define UART_LSR_THRE   PADRE_UART0_STAT_TH
#define UART_LSR_BI     PADRE_UART0_STAT_BR
#define UART_LSR_FE     PADRE_UART0_STAT_FE
#define UART_LSR_PE     PADRE_UART0_STAT_PE
#define UART_LSR_OE     PADRE_UART0_STAT_OV
#define UART_LSR_DR     PADRE_UART0_STAT_DR

#define UART_LCR_EPAR   PADRE_UART0_CTRL_EV
#define UART_LCR_PARITY PADRE_UART0_CTRL_PE
#define UART_IER_RLSI   PADRE_UART0_CTRL_RI
#define UART_IER_THRI   PADRE_UART0_CTRL_TI
#define UART_IER_RDI    PADRE_UART0_CTRL_RI
#define UART_IER_UUE	(PADRE_UART0_CTRL_RI | PADRE_UART0_CTRL_TI)

#define UART_NR		2

#define	PADRE_UART0_CTRL_PE	0x00000020
#define	PADRE_UART0_CTRL_EV	0x00000010

/*
 * Debugging.
 */
#if 0
#define DEBUG_AUTOCONF(fmt...)	printk(fmt)
#else
#define DEBUG_AUTOCONF(fmt...)	do { } while (0)
#endif

#if 0
#define DEBUG_INTR(fmt...)	printk(fmt)
#else
#define DEBUG_INTR(fmt...)	do { } while (0)
#endif

#define PADRE_SIMPLE_UART       14
#define PADRE_SIMPLE_FPGA_UART  15
#define PADRE_P0_UART           16
#define PADRE_P0_TYPE_UART      17

static const char * padre_uart_name[] = 
	{ "padre uart", "padre fpga uart", "p0 uart", "p0 fpga uart" };

struct uart_padre_port {
	struct uart_port	port;
	unsigned int		lcr;
	unsigned char		lsr_break_flag;
};

static unsigned int padre_uart_get_type(void)
{
	return PADRE_SIMPLE_UART;
}

static unsigned int padre_uart_get_clk(void)
{
	return 5000000;
}

static inline unsigned int serial_in(struct uart_padre_port *up, int offset)
{
	return readl(up->port.membase + offset);
}

static inline void
serial_out(struct uart_padre_port *up, int offset, int value)
{
	writel(value, up->port.membase + offset);
}

/*
 * We used to support using pause I/O for certain machines.  We
 * haven't supported this for a while, but just in case it's badly
 * needed for certain old 386 machines, I've left these #define's
 * in....
 */
#define serial_inp(up, offset)		serial_in(up, offset)
#define serial_outp(up, offset, value)	serial_out(up, offset, value)

static void padre_serial_stop_tx(struct uart_port *port, unsigned int tty_stop)
{
	struct uart_padre_port *up = (struct uart_padre_port *)port;

	if (up->lcr & UART_IER_THRI) {
		up->lcr &= ~UART_IER_THRI;
		serial_out(up, UART_IER, up->lcr);
	}
}

static inline void transmit_chars(struct uart_padre_port *up)
{
	struct circ_buf *xmit = &up->port.info->xmit;

	if (up->port.x_char) {
		serial_outp(up, UART_TX, up->port.x_char);
		up->port.icount.tx++;
		up->port.x_char = 0;
		return;
	}
	if (uart_circ_empty(xmit) || uart_tx_stopped(&up->port)) {
		padre_serial_stop_tx(&up->port, 0);
		return;
	}

	while(1) {
		while( ! (serial_in(up, UART_LSR) & UART_LSR_THRE) );
		serial_out(up, UART_TX, xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		up->port.icount.tx++;
		if (uart_circ_empty(xmit))
			break;
	}

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(&up->port);

	DEBUG_INTR("THRE...");

	if (uart_circ_empty(xmit))
		padre_serial_stop_tx(&up->port, 0);
}

static void padre_serial_start_tx(struct uart_port *port, unsigned int tty_start)
{
	struct uart_padre_port *up = (struct uart_padre_port *)port;
	transmit_chars(up);
#if 0
	struct uart_padre_port *up = (struct uart_padre_port *)port;

	if (!(up->lcr & UART_IER_THRI)) {
		up->lcr |= UART_IER_THRI;
		serial_out(up, UART_IER, up->lcr);
	}
#endif
}

static void padre_serial_stop_rx(struct uart_port *port)
{
#if 0
	struct uart_padre_port *up = (struct uart_padre_port *)port;

	up->lcr &= ~UART_IER_RLSI;
	up->port.read_status_mask &= ~UART_LSR_DR;
	serial_out(up, UART_IER, up->lcr);
#endif
}

static void padre_serial_enable_ms(struct uart_port *port)
{
}

static inline void
receive_chars(struct uart_padre_port *up, int *status, struct pt_regs *regs)
{
	struct tty_struct *tty = up->port.info->tty;
	unsigned char ch;
	int max_count = 256;
	char flag;
	int lsr = *status;

	do {
		ch = serial_inp(up, UART_RX);
		flag = TTY_NORMAL;
		up->port.icount.rx++;

#ifdef CONFIG_PADRE_SERIAL_CONSOLE
		if (up->port.line == up->port.cons->index) {
			/* Recover the break flag from console xmit */
			lsr |= up->lsr_break_flag;
			up->lsr_break_flag = 0;
		}
#endif
		if (unlikely(lsr & (UART_LSR_BI | UART_LSR_PE |
				       UART_LSR_FE | UART_LSR_OE))) {
			/*
			 * For statistics only
			 */
			if (lsr & UART_LSR_BI) {
				lsr &= ~(UART_LSR_FE | UART_LSR_PE);
				up->port.icount.brk++;
				/*
				 * We do the SysRQ and SAK checking
				 * here because otherwise the break
				 * may get masked by ignore_status_mask
				 * or read_status_mask.
				 */
				if (uart_handle_break(&up->port))
					goto ignore_char;
			} else if (lsr & UART_LSR_PE)
				up->port.icount.parity++;
			else if (lsr & UART_LSR_FE)
				up->port.icount.frame++;
			if (lsr & UART_LSR_OE)
				up->port.icount.overrun++;

			/*
			 * Mask off conditions which should be ingored.
			 */
			lsr &= up->port.read_status_mask;

			if (lsr & UART_LSR_BI) {
				DEBUG_INTR("handling break....");
				flag = TTY_BREAK;
			} else if (lsr & UART_LSR_PE)
				flag = TTY_PARITY;
			else if (lsr & UART_LSR_FE)
				flag = TTY_FRAME;
		}
		if (uart_handle_sysrq_char(&up->port, ch, regs))
			goto ignore_char;

		uart_insert_char(&up->port, lsr, UART_LSR_OE, ch, flag);
	ignore_char:
		lsr = serial_inp(up, UART_LSR);
	} while ((lsr & UART_LSR_DR) && (max_count-- > 0));
	tty_flip_buffer_push(tty);
	*status = lsr;
}


/*
 * This is the serial driver's interrupt routine.
 *
 * Arjan thinks the old way was overly complex, so it got simplified.
 * Alan disagrees, saying that need the complexity to handle the weird
 * nature of ISA shared interrupts.  (This is a special exception.)
 *
 * In order to handle ISA shared interrupts properly, we need to check
 * that all ports have been serviced, and therefore the ISA interrupt
 * line has been de-asserted.
 *
 * This means we need to loop through all ports. checking that they
 * don't have an interrupt pending.
 */
static irqreturn_t padre_serial_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct uart_padre_port *up = dev_id;

	DEBUG_INTR("padre_serial_interrupt(%d)...", irq);

	unsigned int status = serial_inp(up, UART_LSR);

	DEBUG_INTR("status = %x...", status);

	if (status & UART_LSR_DR)
		receive_chars(up, &status, regs);
#if 0
	if (status & UART_LSR_THRE)
		transmit_chars(up);
#endif

	DEBUG_INTR("end.\n");
	/* FIXME! Was it really ours? */
	return IRQ_HANDLED;
}

static unsigned int padre_serial_tx_empty(struct uart_port *port)
{
	struct uart_padre_port *up = (struct uart_padre_port *)port;
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(&up->port.lock, flags);
	ret = serial_in(up, UART_LSR) & UART_LSR_TEMT ? TIOCSER_TEMT : 0;
	spin_unlock_irqrestore(&up->port.lock, flags);

	return ret;
}

static unsigned int padre_serial_get_mctrl(struct uart_port *port)
{
	return 0;
}

static void padre_serial_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static void padre_serial_break_ctl(struct uart_port *port, int break_state)
{
}

static int padre_serial_startup(struct uart_port *port)
{
	struct uart_padre_port *up = (struct uart_padre_port *)port;
	int retval;

       	retval = request_irq(up->port.irq, padre_serial_interrupt,
                            0, "serial", up);
       	if(retval < 0 ) {
                printk(KERN_ERR"padre can not request irq\n");
                return retval;
        }
	
	
	serial_out(up, 	UART_LSR, 0xff);

	/*
	 * Finally, enable interrupts.  Note: Modem status interrupts
	 * are set via set_termios(), which will be occurring imminently
	 * anyway, so we don't enable them here.
	 */

	up->lcr |= UART_IER_RLSI | UART_IER_RDI;

	serial_outp(up, UART_IER, up->lcr);

	return 0;
}

static void padre_serial_shutdown(struct uart_port *port)
{
	struct uart_padre_port *up = (struct uart_padre_port *)port;

	/*
	 * Disable interrupts from this port
	 */
	up->lcr &= ~UART_IER_UUE;
	serial_outp(up, UART_IER, up->lcr);

	/*
	 * Read data port to reset things, and then unlink from
	 * the IRQ chain.
	 */
	(void) serial_in(up, UART_RX);

        free_irq(port->irq, up);
}

static void
padre_serial_set_termios(struct uart_port *port, struct termios *termios,
		       struct termios *old)
{
	struct uart_padre_port *up = (struct uart_padre_port *)port;
	unsigned char cval;
	unsigned long flags;
	unsigned int baud, quot;
	
	cval = up->lcr;

	if (termios->c_cflag & PARENB)
		cval |= UART_LCR_PARITY;
	else cval &= ~UART_LCR_PARITY;
	if (!(termios->c_cflag & PARODD))
		cval |= UART_LCR_EPAR;
	else cval &= ~UART_LCR_EPAR;

        switch (termios->c_cflag & CBAUD) {
        case B9600:
                baud = 9600;
                break;
        case B19200:
                baud = 19200;
        case B38400:
                baud = 38400;
                break;
        default:
                baud = 9600;
                break;
        }

	quot = up->port.uartclk/(baud * 8) - 1;

#if 0
	if (termios->c_cflag & CRTSCTS)
		up->lcr |= PADRE_UART0_CTRL_FL;
	else up->lcr &= ~PADRE_UART0_CTRL_FL;
#endif


	/*
	 * Ok, we're now changing the port state.  Do it with
	 * interrupts disabled.
	 */
	spin_lock_irqsave(&up->port.lock, flags);

	up->port.read_status_mask = UART_LSR_OE | UART_LSR_THRE | UART_LSR_DR;
	if (termios->c_iflag & INPCK)
		up->port.read_status_mask |= UART_LSR_FE | UART_LSR_PE;
	if (termios->c_iflag & (BRKINT | PARMRK))
		up->port.read_status_mask |= UART_LSR_BI;

	/*
	 * Characteres to ignore
	 */
	up->port.ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		up->port.ignore_status_mask |= UART_LSR_PE | UART_LSR_FE;
	if (termios->c_iflag & IGNBRK) {
		up->port.ignore_status_mask |= UART_LSR_BI;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			up->port.ignore_status_mask |= UART_LSR_OE;
	}

	/*
	 * ignore all characters if CREAD is not set
	 */
	if ((termios->c_cflag & CREAD) == 0)
		up->port.ignore_status_mask |= UART_LSR_DR;

	serial_outp(up, UART_LCR, 0);
	serial_outp(up, UART_LSR, 0xff);
	serial_outp(up, UART_DIV, quot);
	serial_outp(up, UART_LCR, cval);		/* reset DLAB */
	up->lcr = cval;					/* Save LCR */
	spin_unlock_irqrestore(&up->port.lock, flags);
}

static void padre_serial_release_port(struct uart_port *port)
{
}

static int padre_serial_request_port(struct uart_port *port)
{
	return 0;
}

static void padre_serial_config_port(struct uart_port *port, int flags)
{
}

static int
padre_serial_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	return 0;
}

static const char *
padre_serial_type(struct uart_port *port)
{
	return padre_uart_name[port->type-PADRE_SIMPLE_UART];
}

static struct uart_ops padre_serial_pops = {
	.tx_empty	= padre_serial_tx_empty,
	.set_mctrl	= padre_serial_set_mctrl,
	.get_mctrl	= padre_serial_get_mctrl,
	.stop_tx	= padre_serial_stop_tx,
	.start_tx	= padre_serial_start_tx,
	.stop_rx	= padre_serial_stop_rx,
	.enable_ms	= padre_serial_enable_ms,
	.break_ctl	= padre_serial_break_ctl,
	.startup	= padre_serial_startup,
	.shutdown	= padre_serial_shutdown,
	.set_termios	= padre_serial_set_termios,
	.type		= padre_serial_type,
	.release_port	= padre_serial_release_port,
	.request_port	= padre_serial_request_port,
	.config_port	= padre_serial_config_port,
	.verify_port	= padre_serial_verify_port,
};

static struct uart_padre_port padre_serial_ports[UART_NR];

static void __init padre_serial_init_ports(void)
{
	struct uart_padre_port *up;
	static int first = 1;
	int i;

	if (!first)
		return;
	first = 0;

	for (i = 0, up = padre_serial_ports; i < UART_NR; i++, up++) {
		up->port.uartclk  = padre_uart_get_clk();
		if( i == 0) {
			up->port.irq      = 7;
			up->port.membase  = (char *)PADRE_UART1_BASE;
		}
		else {
			up->port.irq      = 8;
			up->port.membase  = (char *)PADRE_UART0_BASE;
		}
		up->port.ops      = &padre_serial_pops;
		up->port.line	  = i;
		up->port.type	  = padre_uart_get_type();
		up->lcr		  = PADRE_UART0_CTRL_TE | PADRE_UART0_CTRL_RE;
	}
}

static void __init padre_serial_register_ports(struct uart_driver *drv)
{
	int i;

	padre_serial_init_ports();

	for (i = 0; i < UART_NR; i++) {
		struct uart_padre_port *up = &padre_serial_ports[i];

		uart_add_one_port(drv, &up->port);
	}
}

#ifdef CONFIG_PADRE_SERIAL_CONSOLE

#define BOTH_EMPTY (UART_LSR_TEMT | UART_LSR_THRE)

/*
 *	Wait for transmitter & holding register to empty
 */
static inline void wait_for_xmitr(struct uart_padre_port *up)
{
	unsigned int status, tmout = 10000;

	/* Wait up to 10ms for the character(s) to be sent. */
	do {
		status = serial_in(up, UART_LSR);

		if (status & UART_LSR_BI)
			up->lsr_break_flag = UART_LSR_BI;

		if (--tmout == 0)
			break;
		udelay(1);
	} while ((status & BOTH_EMPTY) != BOTH_EMPTY);

}

/*
 *	Print a string to the serial port trying not to disturb
 *	any possible real use of the port...
 *
 *	The console_lock must be held when we get here.
 */
static void
padre_serial_console_write(struct console *co, const char *s, unsigned int count)
{
	struct uart_padre_port *up = &padre_serial_ports[co->index];
	unsigned int ier;
	int i;

	/*
	 *	First save the UER then disable the interrupts
	 */
	ier = serial_in(up, UART_IER);

	serial_out(up, UART_IER, ier & ~UART_IER_UUE);

	/*
	 *	Now, do each character
	 */
	for (i = 0; i < count; i++, s++) {
		wait_for_xmitr(up);

		/*
		 *	Send the character out.
		 *	If a LF, also do CR...
		 */
		serial_out(up, UART_TX, *s);
		if (*s == 10) {
			wait_for_xmitr(up);
			serial_out(up, UART_TX, 13);
		}
	}

	/*
	 *	Finally, wait for transmitter to become empty
	 *	and restore the IER
	 */
	wait_for_xmitr(up);
	serial_out(up, UART_IER, ier);
}

static int __init padre_serial_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
	int baud = 9600;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	/*
	 * Check whether an invalid uart number has been specified, and
	 * if so, search for the first available port that does have
	 * console support.
	 */
	if (co->index >= UART_NR)
		co->index = 0;
	port = &padre_serial_ports[co->index].port;
	if (!port->ops)
		return -ENODEV;

	/*
	 * Temporary fix.
	 */
	spin_lock_init(&port->lock);

	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);

	return uart_set_options(port, co, baud, parity, bits, flow);
}

extern struct uart_driver padre_serial_reg;
static struct console padre_serial_console = {
	.name		= "ttyS",
	.write		= padre_serial_console_write,
	.device		= uart_console_device,
	.setup		= padre_serial_console_setup,
	.flags		= CON_PRINTBUFFER,
	.index		= 1,
	.data		= &padre_serial_reg,
};

static int __init padre_serial_console_init(void)
{
	padre_serial_init_ports();
	register_console(&padre_serial_console);
	return 0;
}
console_initcall(padre_serial_console_init);

static int __init padre_serial_late_console_init(void)
{
	if (!(padre_serial_console.flags & CON_ENABLED))
		register_console(&padre_serial_console);
	return 0;
}
late_initcall(padre_serial_late_console_init);

#define PADRE_SERIAL_CONSOLE	&padre_serial_console
#else
#define PADRE_SERIAL_CONSOLE	NULL
#endif

static struct uart_driver padre_serial_reg = {
	.owner			= THIS_MODULE,
	.driver_name		= "serial",
	.devfs_name		= "tts/",
	.dev_name		= "ttyS",
	.major			= TTY_MAJOR,
	.minor			= 64,
	.nr			= UART_NR,
	.cons			= PADRE_SERIAL_CONSOLE,
};

int __init early_serial_setup(struct uart_port *port)
{
	if (port->line >= ARRAY_SIZE(padre_serial_ports))
		return -ENODEV;

	padre_serial_init_ports();
	padre_serial_ports[port->line].port	= *port;
	padre_serial_ports[port->line].port.ops	= &padre_serial_pops;
	return 0;
}

/**
 *	padre_serial_suspend_port - suspend one serial port
 *	@line:  serial line number
 *      @level: the level of port suspension, as per uart_suspend_port
 *
 *	Suspend one serial port.
 */
void padre_serial_suspend_port(int line)
{
	uart_suspend_port(&padre_serial_reg, &padre_serial_ports[line].port);
}

/**
 *	padre_serial_resume_port - resume one serial port
 *	@line:  serial line number
 *      @level: the level of port resumption, as per uart_resume_port
 *
 *	Resume one serial port.
 */
void padre_serial_resume_port(int line)
{
	uart_resume_port(&padre_serial_reg, &padre_serial_ports[line].port);
}

static int __init padre_serial_init(void)
{
	int ret;

	printk(KERN_INFO "Serial: Padre driver $Revision: 1.1.1.1 $ "
		"%d ports\n", (int) UART_NR);

	ret = uart_register_driver(&padre_serial_reg);
	if (ret >= 0)
		padre_serial_register_ports(&padre_serial_reg);

	return ret;
}

static void __exit padre_serial_exit(void)
{
	int i;

	for (i = 0; i < UART_NR; i++)
		uart_remove_one_port(&padre_serial_reg, &padre_serial_ports[i].port);

	uart_unregister_driver(&padre_serial_reg);
}

module_init(padre_serial_init);
module_exit(padre_serial_exit);

EXPORT_SYMBOL(padre_serial_suspend_port);
EXPORT_SYMBOL(padre_serial_resume_port);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Padre serial driver $Revision: 1.1.1.1 $");

MODULE_ALIAS_CHARDEV_MAJOR(TTY_MAJOR);
