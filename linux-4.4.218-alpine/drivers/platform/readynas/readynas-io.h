/*
 * Common ReadyNAS IO driver definitions.
 */
#ifndef __READYNAS_IO_H__
#define __READYNAS_IO_H__
#include "../../gpio/gpiolib.h"
#include "hd44780-lcd.h"

extern struct gpio_chip *rn_gpiochip_find_by_label(const char *label);
extern struct proc_dir_entry *readynas_proc_create(const char *name,
				umode_t mode,
				const struct file_operations *fops,
				void *data);

extern int readynas_gpio_init(void);
extern void readynas_gpio_exit(void);

extern struct i2c_adapter *readynas_i2c_adapter_find(const char **name);

extern const char *rn_i2c_adapters_nct677x[];
extern const char *rn_i2c_adapters_ipch[];

extern bool readynas_io_compatible(char const *nominal);
/*
 * Easy compatibility checking macro.
 * @array - Array of compatible properties
 * @member - Struct member of compatible name string
 * @ptr - Comptible property element
 * @rv - Return value if no compatible property element found
 * @fmt - Error message fmt if no compatible found. Must be "" for null.
 */
#define	__compatible_find(array, member, rv, fmt, ...)	\
	({	\
		int __i;	\
		for (__i = 0; (array)[__i].member; __i++)	\
			if (!(array)[__i].member[0] ||		\
				readynas_io_compatible((array)[__i].member)) \
				break;		\
		if (!(array)[__i].member) {	\
			if ((rv) && *(fmt))	\
				pr_err("%s: " fmt, __func__, ##__VA_ARGS__); \
			return (rv);		\
		}				\
		&(array)[__i];			\
	})
#endif
