#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "readynas.h"

#define LCD_DELAY		50 //ms
#define MAX_LCD_CMDLINE		64
#define LCD_BRIGHT_LOCK_FILE	"/ramfs/.lock_lcd_brightness"

#if defined(__i386__) || defined(__x86_64__)
#  define LCD_PORT		"/sys/devices/system/rn_lcd/rn_lcd0/cmd"
#  define LCD_EXISTS_DIR	"/sys/devices/system/rn_lcd/rn_lcd0"
#  define LCD_RESET		"/sys/devices/system/rn_lcd/rn_lcd0/lcd_reset"
#  define LCD_BACKLIGHT		"/sys/devices/system/rn_lcd/rn_lcd0/lcd_backlight"
#  define LCD_LINE1		"/sys/devices/system/rn_lcd/rn_lcd0/lcd_line1"
#  define LCD_LINE2		"/sys/devices/system/rn_lcd/rn_lcd0/lcd_line2"
#elif __sparc__
#  define LCD_PORT		"/dev/lcd"
#  define LCD_EXISTS_DIR	"/proc/sys/dev/lcd"
#  define LCD_BACKLIGHT		"/proc/sys/dev/lcd/backlight"
#  define IOCTL_LCD_CMD		0x01
#  define IOCTL_LCD_RESET	0x0F
#  define IOCTL_LCD_BACKLIGHT	0x10
#  define LCD_HOME		0x02
#  define LCD_LINE_2		0xC0
#elif __arm__
#  define LCD_PORT		"/proc/LCD"
#endif


int readynas_lcd_is_supported(void)
{
	#ifndef __arm__
	DIR *lcd;

	lcd = opendir(LCD_EXISTS_DIR);
	if( lcd && closedir(lcd) )
		return(0);
	else
		return(1);
	#else
	FILE *lcd = NULL;
        lcd = fopen(LCD_PORT, "r");
        if(lcd && fclose(lcd) )
                return(0);
        else
		return(1);
	#endif
}

int readynas_check_lcd_backlight(void)
{
	#ifndef __arm__
	FILE *backlight;
	int int_buf = 1;

	if( (backlight = fopen(LCD_BACKLIGHT, "r")) )
	{
		fscanf(backlight, "%d", &int_buf);
		fclose(backlight);
	}
	return int_buf;
	#else
	return(0); //ethan how to?
	#endif
}

#ifndef __arm__
#ifndef __sparc__
static void sys_lcd_backlight(int mode)
{
        FILE *lcd = NULL;

        lcd = fopen(LCD_BACKLIGHT, "w");
        if(!lcd) {
                return;
        }

        fprintf(lcd, "%d", mode);

        fclose(lcd);
}

static void sys_lcd_line(const char *name, char *text)
{
        FILE *lcd = NULL;

        lcd = fopen(name, "w");
        if(!lcd) {
                return;
        }

        fprintf(lcd, "%-18.18s", text);

        fclose(lcd);
}
#endif
#endif

void readynas_lcd_off(void)
{

	#ifdef __sparc__
	int lcd;
	int cmd = 0; // Off 

	lcd = open(LCD_PORT, O_RDWR|O_NONBLOCK);
	if(lcd < 0) {
		return;
	}

	ioctl(lcd, IOCTL_LCD_BACKLIGHT, &cmd);
	close(lcd);
	#elif __arm__
	char str_buf[256];
	int lcd;
	lcd = open(LCD_PORT, O_RDWR|O_NONBLOCK);
	if(lcd < 0) {
		return;
	}
	sprintf(str_buf, "echo \'4\' > %s", LCD_PORT);
	system(str_buf);
	close(lcd);
	#else
	sys_lcd_backlight(0);
	#endif
}

void readynas_lock_lcd_brightness(void)
{
	#if defined(__i386__) || defined(__x86_64__)
	FILE *fd;
	if( access(LCD_BRIGHT_LOCK_FILE, F_OK) == 0 )
		return;
	else {
		fd = fopen(LCD_BRIGHT_LOCK_FILE, "w");
		fclose(fd);
	}
	#else
	return;
	#endif
}

void readynas_unlock_lcd_brightness(void)
{
	#if defined(__i386__) || defined(__x86_64__)
	if( access(LCD_BRIGHT_LOCK_FILE, F_OK) == -1 )
		return;
	else {
		unlink(LCD_BRIGHT_LOCK_FILE);
	}
	#else
	return;
	#endif
}

void readynas_lcd_dimmy(void)
{
	#if defined(__i386__) || defined(__x86_64__)
	if( access(LCD_BRIGHT_LOCK_FILE, F_OK) == 0 )
		return;
	sys_lcd_backlight(2);
	#elif __arm__
	char str_buf[256];
	int lcd;
	lcd = open(LCD_PORT, O_RDWR|O_NONBLOCK);
	if(lcd < 0) {
		return;
	}
        sprintf(str_buf, "echo \'3\' > %s", LCD_PORT);
        system(str_buf);
        close(lcd);
	#else
	return;
	#endif
}

void readynas_lcd_sharp(void)
{
	#if defined(__i386__) || defined(__x86_64__)
	/* locking should only lock LCD on */
	sys_lcd_backlight(1);
	#elif __arm__
	char str_buf[256];
	int lcd;
	lcd = open(LCD_PORT, O_RDWR|O_NONBLOCK);
	if(lcd < 0) {
		return;
	}
	sprintf(str_buf, "echo \'3\' > %s", LCD_PORT);
	system(str_buf);
	close(lcd);
	#else
	return;
	#endif
}

void readynas_lcd_clear(void)
{
	#ifdef __sparc__
	int lcd;
	int cmd = 1;

	lcd = open(LCD_PORT, O_RDWR|O_NONBLOCK);
	if(lcd < 0) {
		return;
	}

	ioctl(lcd, IOCTL_LCD_RESET, &cmd);
	close(lcd);
	#elif __arm__
	char str_buf[256];
	int lcd;
	lcd = open(LCD_PORT, O_RDWR|O_NONBLOCK);
	if(lcd < 0) {
		return;
	}
        sprintf(str_buf, "echo \'1 \"                \"\' > %s", LCD_PORT);
        system(str_buf);
	sprintf(str_buf, "echo \'2 \"                \"\' > %s", LCD_PORT);
        system(str_buf);
        close(lcd);
	#else
        FILE *lcd = NULL;

        lcd = fopen(LCD_RESET, "w");
        if(!lcd)
                return;
        fprintf(lcd, "1");
        fclose(lcd);
	#endif
}

void readynas_lcd_disp_line(int line, char *text)
{
	if( !text || (line != 1 && line != 2) )
		return;

	#ifdef __arm__
	//char str_buf[256];
	FILE *lcd = NULL;
	lcd = fopen(LCD_PORT,"w");
	if(!lcd) {
		return;
	}
	//sprintf(str_buf, "echo \'%d \"%-16.16s\"\' > %s", line, text, LCD_PORT);
	//system(str_buf);
	fprintf(lcd, "%d \"%-16.16s\"",line,text);
	fclose(lcd);
	#else
	#ifdef __sparc__
	int lcd;
	lcd = open(LCD_PORT, O_RDWR|O_NONBLOCK);
	if(lcd < 0) {
		return;
	}
	#endif

	if(line == 1) {
		#if defined(__sparc__)
		int cmd = LCD_HOME;
		ioctl(lcd, IOCTL_LCD_CMD, &cmd);
		cmd = 1; // On
		ioctl(lcd, IOCTL_LCD_BACKLIGHT, &cmd);
		dprintf(lcd, "%-16.16s", text);
		#elif defined(__i386__) || defined(__x86_64__)
		sys_lcd_line(LCD_LINE1, text);
		#endif
	}
	else if(line == 2) {
		#ifdef __sparc__
		int cmd = LCD_LINE_2;
		ioctl(lcd, IOCTL_LCD_CMD, &cmd);
		cmd = 1; // On
		ioctl(lcd, IOCTL_LCD_BACKLIGHT, &cmd);
		dprintf(lcd, "%-16.16s", text);
		#elif defined(__i386__) || defined(__x86_64__)
		sys_lcd_line(LCD_LINE2, text);
		#endif
	}

	#ifdef __sparc__
	close(lcd);
	#endif
	#endif
}


