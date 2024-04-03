/* $Id: sparc_ksyms.c,v 1.1.1.1 2010-09-22 00:23:49 jmaggard Exp $
 * arch/sparc/kernel/ksyms.c: Sparc specific ksyms support.
 *
 * Copyright (C) 1996 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 1996 Eddie C. Dost (ecd@skynet.be)
 */

/* Tell string.h we don't want memcpy etc. as cpp defines */
#define EXPORT_SYMTAB_STROPS
#define PROMLIB_INTERNAL

#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/in6.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/irq.h>
#include <linux/syscalls.h>
#ifdef CONFIG_PCI
#include <linux/pci.h>
#endif
#ifdef CONFIG_HIGHMEM
#include <linux/highmem.h>
#endif

#include <linux/skbuff.h>
#include <asm/delay.h>
#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/svr4.h>
#include <asm/head.h>
#include <asm/smp.h>
#include <asm/ptrace.h>
#include <asm/user.h>
#include <asm/uaccess.h>
#include <asm/checksum.h>
#include <asm/a.out.h>
#include <asm/padre_hw_io.h>
#include <asm/bug.h>

extern spinlock_t rtc_lock;

struct poll {
	int fd;
	short events;
	short revents;
};

extern int svr4_getcontext (svr4_ucontext_t *, struct pt_regs *);
extern int svr4_setcontext (svr4_ucontext_t *, struct pt_regs *);
void _sigpause_common (unsigned int set, struct pt_regs *);
extern void (*__copy_1page)(void *, const void *);
extern void __memmove(void *, const void *, __kernel_size_t);
extern void (*bzero_1page)(void *);
extern void *__bzero(void *, size_t);
extern void *__memscan_zero(void *, size_t);
extern void *__memscan_generic(void *, int, size_t);
extern int __memcmp(const void *, const void *, __kernel_size_t);
extern int __strncmp(const char *, const char *, __kernel_size_t);

//extern void bcopy (const char *, char *, int);
extern int __ashrdi3(int, int);
extern int __ashldi3(int, int);
extern int __lshrdi3(int, int);
extern int __muldi3(int, int);
extern int __divdi3(int, int);

extern void dump_thread(struct pt_regs *, struct user *);

/* Private functions with odd calling conventions. */
extern void ___atomic_add(void);
extern void ___atomic_sub(void);
extern void ___set_bit(void);
extern void ___clear_bit(void);
extern void ___change_bit(void);
extern void ___rw_read_enter(void);
extern void ___rw_read_exit(void);
extern void ___rw_write_enter(void);

extern int _Div(int, int);
extern int _Mul(int, int);
extern int _Rem(int, int);
extern unsigned _Udiv(unsigned, unsigned);
extern unsigned _Umul(unsigned, unsigned);
extern unsigned _Urem(unsigned, unsigned);



/* Alias functions whose names begin with "." and export the aliases.
 * The module references will be fixed up by module_frob_arch_sections.
 */
#ifdef CONFIG_PADRE

#define DOT_ALIAS2(__ret, __x, __arg1, __arg2) \
        extern __ret __x(__arg1, __arg2); \
        asm(".weak " #__x);\
        asm(#__x "=." #__x);

#else

#define DOT_ALIAS2(__ret, __x, __arg1, __arg2) \
	extern __ret __x(__arg1, __arg2) \
	             __attribute__((weak, alias("." # __x)));

#endif

DOT_ALIAS2(int, div, int, int)
DOT_ALIAS2(int, mul, int, int)
DOT_ALIAS2(int, rem, int, int)
DOT_ALIAS2(unsigned, udiv, unsigned, unsigned)
DOT_ALIAS2(unsigned, umul, unsigned, unsigned)
DOT_ALIAS2(unsigned, urem, unsigned, unsigned)

#undef DOT_ALIAS2

/* used by various drivers */
EXPORT_SYMBOL(kernel_thread);
#ifdef CONFIG_DEBUG_SPINLOCK
#ifdef CONFIG_SMP
EXPORT_SYMBOL(_do_spin_lock);
EXPORT_SYMBOL(_do_spin_unlock);
EXPORT_SYMBOL(_spin_trylock);
EXPORT_SYMBOL(_do_read_lock);
EXPORT_SYMBOL(_do_read_unlock);
EXPORT_SYMBOL(_do_write_lock);
EXPORT_SYMBOL(_do_write_unlock);
#endif
#else
// XXX find what uses (or used) these.
// EXPORT_SYMBOL_PRIVATE(_rw_read_enter);
// EXPORT_SYMBOL_PRIVATE(_rw_read_exit);
// EXPORT_SYMBOL_PRIVATE(_rw_write_enter);
EXPORT_SYMBOL(___rw_read_enter);
EXPORT_SYMBOL(___rw_read_exit);
EXPORT_SYMBOL(___rw_write_enter);
#endif
/* semaphores */
EXPORT_SYMBOL(__up);
EXPORT_SYMBOL(__down);
EXPORT_SYMBOL(__down_trylock);
EXPORT_SYMBOL(__down_interruptible);

/* Atomic operations. */
EXPORT_SYMBOL(___atomic_add);
EXPORT_SYMBOL(___atomic_sub);


/* Bit operations. */
EXPORT_SYMBOL(___set_bit);
EXPORT_SYMBOL(___clear_bit);
EXPORT_SYMBOL(___change_bit);

/* Per-CPU information table */
EXPORT_PER_CPU_SYMBOL(__cpu_data);

#ifdef CONFIG_SMP
/* IRQ implementation. */
EXPORT_SYMBOL(synchronize_irq);

/* Misc SMP information */
EXPORT_SYMBOL(__cpu_number_map);
EXPORT_SYMBOL(__cpu_logical_map);

/* CPU online map and active count. */
EXPORT_SYMBOL(cpu_online_map);
EXPORT_SYMBOL(phys_cpu_present_map);
#endif

EXPORT_SYMBOL(__udelay);
EXPORT_SYMBOL(__ndelay);
EXPORT_SYMBOL(rtc_lock);
/* PADRE_TODO
EXPORT_SYMBOL(request_fast_irq);
EXPORT_SYMBOL(io_remap_page_range);
EXPORT_SYMBOL(io_remap_pfn_range);
*/
  /* P3: iounit_xxx may be needed, sun4d users */
/* EXPORT_SYMBOL(iounit_map_dma_init); */
/* EXPORT_SYMBOL(iounit_map_dma_page); */

EXPORT_SYMBOL(___xchg32);
EXPORT_SYMBOL(enable_irq);
EXPORT_SYMBOL(disable_irq);
EXPORT_SYMBOL(__irq_itoa);

#ifdef CONFIG_PCI
EXPORT_SYMBOL(insb);
EXPORT_SYMBOL(outsb);
EXPORT_SYMBOL(insw);
EXPORT_SYMBOL(outsw);
EXPORT_SYMBOL(insl);
EXPORT_SYMBOL(outsl);
EXPORT_SYMBOL(pci_alloc_consistent);
EXPORT_SYMBOL(pci_free_consistent);
EXPORT_SYMBOL(pci_map_single);
EXPORT_SYMBOL(pci_unmap_single);
EXPORT_SYMBOL(pci_dma_sync_single_for_cpu);
EXPORT_SYMBOL(pci_dma_sync_single_for_device);
EXPORT_SYMBOL(pci_dma_sync_sg_for_cpu);
EXPORT_SYMBOL(pci_dma_sync_sg_for_device);
EXPORT_SYMBOL(pci_map_sg);
EXPORT_SYMBOL(pci_unmap_sg);
EXPORT_SYMBOL(pci_map_page);
EXPORT_SYMBOL(pci_unmap_page);
/* Actually, ioremap/iounmap are not PCI specific. But it is ok for drivers. */
//EXPORT_SYMBOL(ioremap);
//EXPORT_SYMBOL(iounmap);
#endif

/* in arch/sparc/mm/highmem.c */
#ifdef CONFIG_HIGHMEM
EXPORT_SYMBOL(kmap_atomic);
EXPORT_SYMBOL(kunmap_atomic);
#endif

/* Solaris/SunOS binary compatibility */
EXPORT_SYMBOL(svr4_setcontext);
EXPORT_SYMBOL(svr4_getcontext);
EXPORT_SYMBOL(_sigpause_common);

EXPORT_SYMBOL(dump_thread);

/* sparc library symbols */
//EXPORT_SYMBOL(bcopy);
EXPORT_SYMBOL(memchr);
EXPORT_SYMBOL(memscan);
EXPORT_SYMBOL(strlen);
EXPORT_SYMBOL(strnlen);
EXPORT_SYMBOL(strcpy);
EXPORT_SYMBOL(strncpy);
EXPORT_SYMBOL(strcat);
EXPORT_SYMBOL(strncat);
EXPORT_SYMBOL(strcmp);
EXPORT_SYMBOL(strncmp);
EXPORT_SYMBOL(strchr);
EXPORT_SYMBOL(strrchr);
EXPORT_SYMBOL(strpbrk);
EXPORT_SYMBOL(strstr);

/* Special internal versions of library functions. */
EXPORT_SYMBOL(__copy_1page);
EXPORT_SYMBOL(__memcpy);
EXPORT_SYMBOL(__memset);
EXPORT_SYMBOL(bzero_1page);
EXPORT_SYMBOL(__bzero);
EXPORT_SYMBOL(__memscan_zero);
EXPORT_SYMBOL(__memscan_generic);
EXPORT_SYMBOL(__memcmp);
EXPORT_SYMBOL(__strncmp);
EXPORT_SYMBOL(__memmove);

/* Moving data to/from userspace. */
EXPORT_SYMBOL(__copy_user);
EXPORT_SYMBOL(__strncpy_from_user);

/* Networking helper routines. */
EXPORT_SYMBOL(__csum_partial_copy_sparc_generic);
EXPORT_SYMBOL(csum_partial);

/* Cache flushing.  */
//EXPORT_SYMBOL(sparc_flush_page_to_ram);

/* For when serial stuff is built as modules. */
//EXPORT_SYMBOL(sun_do_break);

EXPORT_SYMBOL(__ret_efault);

EXPORT_SYMBOL(memcmp);
EXPORT_SYMBOL(memcpy);
EXPORT_SYMBOL(memset);
EXPORT_SYMBOL(memmove);
EXPORT_SYMBOL(__ashrdi3);
EXPORT_SYMBOL(__ashldi3);
EXPORT_SYMBOL(__lshrdi3);
EXPORT_SYMBOL(__muldi3);
EXPORT_SYMBOL(__divdi3);

EXPORT_SYMBOL(_Rem);
EXPORT_SYMBOL(_Urem);
EXPORT_SYMBOL(_Mul);
EXPORT_SYMBOL(_Umul);
EXPORT_SYMBOL(_Div);
EXPORT_SYMBOL(_Udiv);

EXPORT_SYMBOL(rem);
EXPORT_SYMBOL(urem);
EXPORT_SYMBOL(mul);
EXPORT_SYMBOL(umul);
EXPORT_SYMBOL(div);
EXPORT_SYMBOL(udiv);

#ifdef CONFIG_DEBUG_BUGVERBOSE
EXPORT_SYMBOL(do_BUG);
#endif

/* Sun Power Management Idle Handler */
//EXPORT_SYMBOL(pm_idle);

/* Binfmt_misc needs this */
EXPORT_SYMBOL(sys_close);

extern unsigned long cpu_id;
extern irq_desc_t irq_desc[NR_IRQS];
extern void init_eth_desc ( void );
extern unsigned long alloc_eth_desc ( void );
extern void free_eth_desc(unsigned long addr);
extern unsigned int g_sys_type;
extern unsigned int g_sys_asicid;
extern void ics_get_freq(unsigned long *fcpu, unsigned long *fpci);
extern u32 __udivdi3(u64 a, u32 b);
extern unsigned long *register_padre_ecounter(char *name, unsigned long *counter);
extern int unregister_padre_ecounter(unsigned long *counter);
extern void LCD_display(unsigned char * info);

extern struct usb_ecounter usb_ec;

/* PADRE SYMBOLS */
EXPORT_SYMBOL(cpu_id);
EXPORT_SYMBOL(padre_boot_param);
EXPORT_SYMBOL(init_eth_desc);
EXPORT_SYMBOL(alloc_eth_desc);
EXPORT_SYMBOL(free_eth_desc);
EXPORT_SYMBOL(PAGE_NONCACHE_MARK);
EXPORT_SYMBOL(padre_led_handler);
EXPORT_SYMBOL(g_sys_type);
EXPORT_SYMBOL(g_sys_asicid);
EXPORT_SYMBOL(__udivdi3);
EXPORT_SYMBOL(padre_NSPIO);
EXPORT_SYMBOL(irq_desc);
EXPORT_SYMBOL(register_padre_ecounter);
EXPORT_SYMBOL(unregister_padre_ecounter);
EXPORT_SYMBOL(usb_ec);
EXPORT_SYMBOL(LCD_display);

extern seqlock_t xtime_lock;
EXPORT_SYMBOL(xtime_lock);

extern void (* padre_file_read_func)(struct file * in_file, loff_t *ppos,
                             read_descriptor_t * desc,
                             read_actor_t actor);
extern void flush_cache_dma(int direction);
extern void clean_dcache_range(unsigned long start, unsigned long end);
extern int padre_cachefix;
extern char uevent_helper[];
extern int (* memcpy_toiovec_func)(struct iovec *iov, unsigned char *kdata, int len);
EXPORT_SYMBOL(padre_file_read_func);
EXPORT_SYMBOL(flush_cache_dma);
EXPORT_SYMBOL(clean_dcache_range);
EXPORT_SYMBOL(padre_cachefix);
EXPORT_SYMBOL(memcpy_toiovec_func);
EXPORT_SYMBOL(uevent_helper);

extern void machine_power_off(void);
EXPORT_SYMBOL(machine_power_off);

extern struct bad_block_table bbt;
EXPORT_SYMBOL(bbt);

#ifdef CONFIG_HOTPLUG
#include <asm/hotplug.h>
extern padre_hotplug_thread_t * padre_hotplug_thread_init(void (*run) (void *), void *data, const char *name);
extern void padre_wakeup_hotplug_thread(padre_hotplug_thread_t *thread);
EXPORT_SYMBOL(padre_hotplug_thread_init);
EXPORT_SYMBOL(padre_wakeup_hotplug_thread);
#endif

extern unsigned long ec_tcp_retrans;
extern unsigned long ec_tcp_retrans_unrecover;
extern void flush_dcache_range(unsigned long start, unsigned long end);
EXPORT_SYMBOL(ec_tcp_retrans);
EXPORT_SYMBOL(ec_tcp_retrans_unrecover);
EXPORT_SYMBOL(flush_dcache_range);

extern int (* check_sock_func1)(struct sock *sk);
EXPORT_SYMBOL(check_sock_func1);

extern unsigned int tso_buffer_ratio;
EXPORT_SYMBOL(tso_buffer_ratio);

