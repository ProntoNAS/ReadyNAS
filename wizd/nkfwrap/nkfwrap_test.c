#include <stdio.h>
#include "nkfwrap.h"

int main()
{
	char inbuf[1024];
	char outbuf[1024];

	puts(nkf_version());

	fgets(inbuf, sizeof(inbuf), stdin);
	inbuf[strlen(inbuf) - 1] = '\0'; // deletes \n

	nkf(inbuf, outbuf, sizeof(outbuf), "-e");
	puts(outbuf);

	nkf(inbuf, outbuf, sizeof(outbuf), "-Sxe");
	puts(outbuf);

	nkf(inbuf, outbuf, sizeof(outbuf), "-e --url-input --cap-input");
	puts(outbuf);

	return 0;
}
