#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "wizd_aviread.h"

int print_avi_info(char *fname)
{
	FILE *fp;
	MainAVIHeader avih;
	AVIStreamHeader avish;
	int listlen, len;
	FOURCC fcc;

	fp = fopen(fname, "rb");
	if (fp == NULL) return -1;

	listlen = read_avi_main_header(fp, &avih);
	printf("listlen: %d\n", listlen);

	printf("%s :  ", fname);
	printf("%lu x %lu  ", avih.dwWidth, avih.dwHeight);

	//printf("1us/frame: %lu. -> fps: %g\n", avih.dwMicroSecPerFrame
	//	, 1000000.0/avih.dwMicroSecPerFrame);
	//printf("frames: %lu -> time: %s\n", avih.dwTotalFrames
	//	, format_time((unsigned long)((double)avih.dwTotalFrames * avih.dwMicroSecPerFrame / 1000000))
	//);
	//printf("\n");

	while ((len = read_next_chunk(fp, &fcc)) > 0) {
		printf("--- %s\n", str_fourcc(fcc));
		if (fcc != SYM_LIST) break;
		if (read_avi_stream_header(fp, &avish, len) < 0) break;
		printf("stream:\n");
		printf("fccType: %s\n", str_fourcc(avish.fccType));
		printf("fccHandler: %s (%s)\n", str_fourcc(avish.fccHandler), str_fourcc(avish.dwReserved1));
		printf("rate: %d\n", avish.dwRate);
		printf("\n");
		if (avish.fccType == SYM_VIDS) {
			printf("V: %s ", str_fourcc(avish.fccHandler));
			if (avish.fccHandler != avish.dwReserved1) {
				printf("(%s) ", str_fourcc(avish.dwReserved1));
			}
			printf(" ");
		} else if (avish.fccType == SYM_AUDS) {
			printf("A: %s ", str_acodec(avish.fccHandler));
			if (avish.fccHandler != avish.dwReserved1) {
				printf("(%s) ", str_acodec(avish.dwReserved1));
			}
			printf(" ");
		}
	}

	do {
		int flag = 0;
		DWORD riff_type;

		printf("--- %s\n", str_fourcc(fcc));
		if (fcc == SYM_LIST) {
			riff_type = read_next_sym(fp);
			if (riff_type == -1) {
				printf("?\n");
				return -1;
			}
			if (riff_type == SYM_MOVI) {
				DWORD old = 0;
				int i;
				printf("----- %s\n", str_fourcc(riff_type));
				for (i=0; i</*avih.dwTotalFrames/ */100 && (len = read_next_chunk(fp, &fcc)) >= 0; i++) {
					len = (len + 1)/ 2 * 2;
					fseek(fp, len, SEEK_CUR);
					printf("fcc: %s\n", str_fourcc(fcc));
					printf("fcc&: %s, ", str_fourcc(fcc & 0x0000ffff));
					printf("old: %s\n", str_fourcc(old));
					if (i != 0 && (fcc & 0x0000ffff) != old) {
						flag = 1;
						break;
					}
					old = fcc & 0x0000ffff;
				}
				printf("%s", flag?"[I]":"[NI]");
				break;
			} else {
				printf("----- %s..\n", str_fourcc(riff_type));
				fseek(fp, len - sizeof(riff_type), SEEK_CUR);
			}
		} else {
			printf("FOURCC: %s\n", str_fourcc(fcc));
			fseek(fp, len, SEEK_CUR);
		}
	} while ((len = read_next_chunk(fp, &fcc)) > 0);

	fclose(fp);
	printf("\n");

	return 0;
}

int main(int argc, char *argv[])
{
	int i;
	for (i = 1; i<argc; i++) {
		print_avi_info(argv[i]);
	}
	return 0;
}

