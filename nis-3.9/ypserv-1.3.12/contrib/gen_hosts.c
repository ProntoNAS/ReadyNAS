/*
** Generates a hosts file with a lot of entrys
*/

#include <stdio.h>

void main()
{
  int i;

  for(i = 1; i <= 30000; i++)
    printf("10.0.%d.%d\thost%d\n",(i/256)%256,i%255+1,i);
}
