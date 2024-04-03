/*
** Copyright (c) 1996 Thorsten Kukuk
*/

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>

void main(int argc, char *argv[])
{
  struct passwd *pwd;

  if(argc != 2)
    {
      fprintf(stderr,"Usage: getwpnam username\n");
      exit(1);
    }

  pwd=getpwnam(argv[1]);

  if(pwd != NULL)
    {
      printf("name.....: [%s]\n",pwd->pw_name);
      printf("password.: [%s]\n",pwd->pw_passwd);
      printf("user id..: [%d]\n", pwd->pw_uid);
      printf("group id.: [%d]\n",pwd->pw_gid);
      printf("gecos....: [%s]\n",pwd->pw_gecos);
      printf("directory: [%s]\n",pwd->pw_dir);
      printf("shell....: [%s]\n",pwd->pw_shell);
    }
  else
    fprintf(stderr,"User \"%s\" not found!\n",argv[1]);

  exit(0);
}
