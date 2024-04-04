/*
 * it87-gpio.h
 *
 * it87-gpio deivce-specific "control" command definitions.
 *
 * Author: <hiro.sugawara@netgear.com>
 */

#define IT8732_CMD_GPIO_IRQ_EXT_ROUTING	0
#define	IT8732_CMD_GPIO_POLARITY	1
#define	IT8732_CMD_GPIO_IRQ_GET		2
#define	IT8732_CMD_GPIO_IRQ_SET		3
#define	IT8732_CMD_GPIO_IRQ_REGISTER	0x100

#define	IS_IT8732_CMD_GPIO_IRQ_REGISTER(cmd)	\
		(((cmd) & ~0xff) == IT8732_CMD_GPIO_IRQ_REGISTER)
#define	IRQ_IT8732_CMD_GPIO_IRQ_REGISTER(cmd)	((cmd & 0xff))
#define	MK_IT8732_CMD_GPIO_IRQ_REGISTER(cmd)	\
		((cmd) | IT8732_CMD_GPIO_IRQ_REGISTER)
