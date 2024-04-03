#include <time.h>
#include <unistd.h>

void readynas_msleep(int msec)
{
	usleep(msec*1000);
}
