static char *
stpcpy (char *dest, char *src)
{
  char *char_ptr;

  char_ptr = dest;

  while (*src)
    {
      *char_ptr++ = *src++;
    }

  *char_ptr = '\0';

  return (src);
}
