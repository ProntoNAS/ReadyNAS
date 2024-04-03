#include <stdio.h>
#include <libreadynas/readynas.h>

/* COMPILE: gcc -lreadynas -o lcd-test lcd-test.c */
int main(int argc, char *argv, char *env)
{
	int i = 0;
	int rc = 0;

/*
	printf("Start test LCD functions\n");

	readynas_lcd_clear();
	readynas_lcd_sharp();

	readynas_lcd_disp_line(1, "192.168.168.168");
	readynas_lcd_disp_line(2, "C: 400/500G free");
	readynas_lcd_off();
	readynas_lcd_sharp();
	readynas_lcd_dimmy();
	readynas_lcd_sharp();

	printf("Start test Disk LED functions\n");
	for(i=1; i<=12; i++)
		readynas_lcd_disp_disk_blank(i);

	for(i=1; i<=12; i++)
		readynas_lcd_disp_disk_std(i);

	for(i=1; i<=12; i++)
		readynas_lcd_disp_disk_failed(i);

	for(i=1; i<=12; i++)
		readynas_lcd_disp_disk_blank(i);

        for(i=1; i<=12; i++)
                readynas_lcd_disp_disk_std(i);
*/

	printf("Start test PSU functions\n");

	rc = get_psu_power_status(1);
	if(rc == 1)
		printf("PSU 1 is GOOD: %d\n", rc);
	else
		printf("PSU 1 is BAD: %d\n", rc);
		
	rc = get_psu_power_status(2);
	if(rc == 1)
		printf("PSU 2 is GOOD: %d\n", rc);
	else
		printf("PSU 2 is BAD: %d\n", rc);
	
	printf("The End.\n");
}
