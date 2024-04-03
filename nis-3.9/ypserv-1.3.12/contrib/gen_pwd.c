/*
** Generates a passwd file with a lot of entrys
*/

#include <stdio.h>

void main()
{
  int i;

  for(i = 2001; i <= 32000; i++)
    printf("usr%d:*:%d:101:Test User %d:/tmp:/usr/local/bin/no_login\n",
	   i,i,i-2000);
}
