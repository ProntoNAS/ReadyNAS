 /*
   * board/annapurna-labs/common/lcd.c
   *
   * Thie file contains implementations for the LCD mechanism
   *
   * Copyright (C) 2013 Annapurna Labs Ltd.
   *
   * This program is free software; you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation; either version 2 of the License, or
   * (at your option) any later version.
   *
   * This program is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   * GNU General Public License for more details.
   *
   * You should have received a copy of the GNU General Public License
   * along with this program; if not, write to the Free Software
   * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   */

#include "lcd.h"
#include <common.h>
#include <asm/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

#define LCD_NUM_ROWS	2
#define LCD_NUM_COLS	16



#define DATA_TYPE_8BIT	1
#define DATA_TYPE_4BIT	0

static char biDataType = DATA_TYPE_4BIT;

#define GPIO_RW	41
#define GPIO_E	42
#define GPIO_RS	35
#define GPIO_D7	39
#define GPIO_D6	38
#define GPIO_D5	37
#define GPIO_D4	36

static void Delay1Ms(unsigned int d)   
{   
    udelay(d*1000);
}

static void pin_write (char gpio, char value)
{
	gpio_set_value (gpio, value);
	udelay(1);
}

static void data_write (char value)
{
	if (biDataType == DATA_TYPE_4BIT)
	{
		gpio_set_value (GPIO_D7, (value & (1 << 7)) ? 1 : 0);
		gpio_set_value (GPIO_D6, (value & (1 << 6)) ? 1 : 0);
		gpio_set_value (GPIO_D5, (value & (1 << 5)) ? 1 : 0);
		gpio_set_value (GPIO_D4, (value & (1 << 4)) ? 1 : 0);
	}
	udelay(1);
}

static char data_read (void)
{
    char c = 0;

    c  =  gpio_get_value(GPIO_D7) ? 0x08 : 0;
    c |= (gpio_get_value(GPIO_D6) ? 0x04 : 0);
    c |= (gpio_get_value(GPIO_D5) ? 0x02 : 0);
    c |= (gpio_get_value(GPIO_D4) ? 0x01 : 0);

    return c;
}

void LCMBusyCheck(void)   
{   
    char c;
    gpio_direction_input(GPIO_D7);
    gpio_direction_input(GPIO_D6);
    gpio_direction_input(GPIO_D5);
    gpio_direction_input(GPIO_D4);
    while (1)   
    {   
        pin_write (GPIO_RS, 0);
        pin_write (GPIO_RW, 1);
        pin_write (GPIO_E, 1); 
        c = (data_read () << 4);
        pin_write (GPIO_E, 0);
        Delay1Ms(1);
        pin_write (GPIO_E, 1); 
        c = (data_read ());
        pin_write (GPIO_E, 0);
        if ((c & 0x80) == 0x00)  break;   
        
        Delay1Ms(1); 
    }   
    gpio_direction_output(GPIO_D7, 0);
    gpio_direction_output(GPIO_D6, 0);
    gpio_direction_output(GPIO_D5, 0);
    gpio_direction_output(GPIO_D4, 0);
}

static void LCMWriteCommand(unsigned char d)   
{   
    LCMBusyCheck();   
    pin_write (GPIO_RS, 0);
    pin_write (GPIO_RW, 0);  
    data_write (d);
    pin_write(GPIO_E, 1);
    pin_write(GPIO_E, 0); 
    if (biDataType == DATA_TYPE_4BIT)
    {   
        pin_write (GPIO_RS, 0);
        pin_write (GPIO_RW, 0);  
        data_write (d << 4);
        pin_write(GPIO_E, 1);
        pin_write(GPIO_E, 0); 
    }   
    Delay1Ms(10); 
}

static void LCMWriteData(unsigned char d)   
{   
    LCMBusyCheck();   

    pin_write (GPIO_RS, 1);
    pin_write (GPIO_RW, 0);  
    data_write (d);
    pin_write(GPIO_E, 1);
    pin_write(GPIO_E, 0);   
    if (biDataType == DATA_TYPE_4BIT)
    {   
        pin_write (GPIO_RS, 1);
        pin_write (GPIO_RW, 0);  
        data_write (d << 4);
        pin_write(GPIO_E, 1);
        pin_write(GPIO_E, 0);   
    }   
    Delay1Ms(1); 
}  

static void LCMWriteInitCommand(unsigned char d)   
{
    pin_write (GPIO_RS, 0);
    pin_write (GPIO_RW, 0);  
    data_write (d);
    pin_write(GPIO_E, 1);
    pin_write(GPIO_E, 0);   
    if (biDataType == DATA_TYPE_4BIT)
    {   
				pin_write (GPIO_RS, 0);
				pin_write (GPIO_RW, 0);  
        data_write (d << 4);
				pin_write(GPIO_E, 1);
    		pin_write(GPIO_E, 0);
    }   
    
    Delay1Ms(10); 
}

static void LCMInit(void)
{
    
        LCMWriteInitCommand(0x20);
        LCMWriteInitCommand(0x20);
        LCMWriteInitCommand(0x20);

}

/*****************************************************************************/
/*****************************************************************************/
static void lcd_command(uint8_t val)
{
	debug("%s(%02x)\n", __func__, val);


	LCMWriteCommand(val);

}

/*****************************************************************************/
/*****************************************************************************/
static void lcd_data(uint8_t val)
{
	debug("%s(%02x)\n", __func__, val);


	LCMWriteData(val);

}

/*****************************************************************************/
/*****************************************************************************/
int lcd_init(void)
{
  LCMInit();
  
	lcd_display_clear();
	lcd_command(0x28); /* 8 bit two row 5*7 dot */
	udelay(53);
	lcd_display_control(1, 0, 0);
	lcd_entry_mode_set(1, 0);
	lcd_cursor_set(0, 0);
	
	return 0;
}

/*****************************************************************************/
/*****************************************************************************/
void lcd_display_clear(void)
{
	lcd_command(0x1);
	udelay(10000);
}

/*****************************************************************************/
/*****************************************************************************/
void lcd_return_home(void)
{
	lcd_command(0x2);
	udelay(10000);
}

/*****************************************************************************/
/*****************************************************************************/
void lcd_entry_mode_set(int l2r, int scroll)
{
	lcd_command(0x4 | (l2r << 1) | scroll);
	udelay(53);
}

/*****************************************************************************/
/*****************************************************************************/
void lcd_display_control(int display_on, int cursor_on, int blink_on)
{
	lcd_command(0x8 | (display_on << 2) | (cursor_on << 1) | blink_on);
	udelay(53);
}

/*****************************************************************************/
/*****************************************************************************/
void lcd_cursor_set(int row, int col)
{
	lcd_command(0x80 | (row << 6) | col);
	udelay(53);
}

/*****************************************************************************/
/*****************************************************************************/
void lcd_putc(const char c)
{
	lcd_data(c);
	udelay(53);
}

/*****************************************************************************/
/*****************************************************************************/
void lcd_puts(const char *s)
{
	for (; *s; s++)
		lcd_putc(*s);
}

/*****************************************************************************/
/*****************************************************************************/
void lcd_printf(const char *fmt, ...)
{
	va_list args;
	static char buf[80];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	lcd_puts(buf);
}

