
#include "io.h"

unsigned int GPIO_BASE_ADDR;


#define GPIO_WRITE(val, reg)	outl((val), (reg) + GPIO_BASE_ADDR)
#define GPIO_READ(reg)		inl((reg) + GPIO_BASE_ADDR)

static const unsigned char regs[4][3] = {
	{0x00, 0x30, 0x40},	/* USE_SEL[1-3] offsets */
	{0x04, 0x34, 0x44},	/* IO_SEL[1-3] offsets */
	{0x0c, 0x38, 0x48},	/* LVL[1-3] offsets */
	{0x18, 0x18, 0x18},	/* GPIO_BLINK offset */
};


int hw_read_bit(int reg, unsigned nr)
{
	unsigned int data;
	int reg_nr = nr/32;
	int bit = nr & 0x1f;

	data = GPIO_READ(regs[reg][reg_nr]);

	return data & (1 << bit) ? 1 : 0;
}

void hw_write_bit(int reg, unsigned nr, int val)
{
	unsigned int data;
	int reg_nr = nr/32;
	int bit = nr & 0x1f;

	data = GPIO_READ(regs[reg][reg_nr]);
	data = (data & ~(1 << bit)) | (val << bit);
	GPIO_WRITE(data, regs[reg][reg_nr % 3]);
}

int hw_read_byte(int reg, int reg_index)
{
	return GPIO_READ(regs[reg][reg_index % 3]);
}

void hw_write_byte(int reg, int reg_index, int val)
{
	GPIO_WRITE(val, regs[reg][reg_index % 3]);
}


