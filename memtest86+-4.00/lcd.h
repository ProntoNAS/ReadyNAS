
#ifndef _LCD_H
#define _LCD_H

void G2_4bay_lcd_main(int devid);
void lcd_main(int devid);
void show_lcd_line(int line, char *c);
void oled_init(void);
void lcd_reset(void);

void set_all_leds_off(void);
void set_all_leds_on(void);
void set_led_oppsite(int count);


#endif

