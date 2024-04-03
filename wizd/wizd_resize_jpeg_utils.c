/****************************************************/
/* Pour cree une image a partir d'un fichier JPEG   */
/* image_from_jpeg.c                                */
/*                                                  */
/* Ecrit par : Daniel Lacroix (all rights reserved) */
/* adapte de imlib qui est sous LGPL                */
/* http://www.labs.redhat.com/imlib/                */
/****************************************************/
# ifdef RESIZE_JPEG
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <string.h>
#include <math.h>

#define USE_INTEGER

#include "wizd_resize_jpeg.h"

jmp_buf setjmp_buffer;

static void
g_JPEGFatalErrorHandler (j_common_ptr cinfo)
{
    cinfo->err->output_message (cinfo);
    longjmp (setjmp_buffer, 1);
    return;
}

image *
image_new_from_jpeg (char *file_name)
{
    image *vimage;
    FILE *file;
    unsigned char *line[16], *ptr;
    int x, y, i, w, h, ofs;
    int maxbuf;
    struct jpeg_error_mgr pub;
    struct jpeg_decompress_struct cinfo;

    if ((file = fopen (file_name, "r")) == NULL)
		return NULL;

    cinfo.err = jpeg_std_error (&pub);
    jpeg_create_decompress (&cinfo);
    pub.error_exit = g_JPEGFatalErrorHandler;

    /* error handler to longjmp to, we want to preserve signals */
    if (setjmp (setjmp_buffer)) {
		jpeg_destroy_decompress (&cinfo);
		fclose (file);
		return NULL;
    }

    jpeg_stdio_src (&cinfo, file);
    jpeg_read_header (&cinfo, TRUE);
    cinfo.do_fancy_upsampling = FALSE;
    cinfo.do_block_smoothing = FALSE;
    jpeg_start_decompress (&cinfo);
    w = cinfo.output_width;
    h = cinfo.output_height;
    vimage = image_new (w, h);
    if (!vimage) {
		jpeg_destroy_decompress (&cinfo);
		fclose (file);
		return NULL;
    }

	if (cinfo.rec_outbuf_height > 16) {
		fprintf (stderr,
			 "ERROR image_from_jpeg : (image_from_jpeg.c) JPEG uses line buffers > 16. Cannot load.\n");
		image_free (vimage);
		fclose (file);
		return NULL;
	}

    maxbuf = vimage->width * vimage->height;
	if (cinfo.output_components == 3) {
		ofs = 0;
		if ((ptr = (char *) malloc (w * 3 * cinfo.rec_outbuf_height)) == NULL) {
			perror ("malloc failed ");
			exit (1);
		}

		for (y = 0; y < h; y += cinfo.rec_outbuf_height) {
			for (i = 0; i < cinfo.rec_outbuf_height; i++)
				line[i] = ptr + (w * 3 * i);

			jpeg_read_scanlines (&cinfo, line, cinfo.rec_outbuf_height);
			for (x = 0; x < w * cinfo.rec_outbuf_height; x++) {
				if (ofs < maxbuf) {
					vimage->buf[ofs] =
					COL (ptr[x + x + x], ptr[x + x + x + 1],
						 ptr[x + x + x + 2]);
					ofs++;
				}
			}
		}

		free (ptr);
	} else if (cinfo.output_components == 1) {
		ofs = 0;
		for (i = 0; i < cinfo.rec_outbuf_height; i++) {
			if ((line[i] = (unsigned char *) malloc (w)) == NULL) {
				int t = 0;

				for (t = 0; t < i; t++)
					free (line[t]);

				jpeg_destroy_decompress (&cinfo);
				image_free (vimage);
				fclose (file);
				return NULL;
			}
		}

		for (y = 0; y < h; y += cinfo.rec_outbuf_height) {
			jpeg_read_scanlines (&cinfo, line, cinfo.rec_outbuf_height);
			for (i = 0; i < cinfo.rec_outbuf_height; i++) {
				for (x = 0; x < w; x++) {
					vimage->buf[ofs++] = COL (line[i][x], line[i][x], line[i][x]);
				}
			}
		}

		for (i = 0; i < cinfo.rec_outbuf_height; i++)
			free (line[i]);
	}

    jpeg_finish_decompress (&cinfo);
    jpeg_destroy_decompress (&cinfo);

    fclose (file);
    return vimage;
}

image *
image_new_from_jpeg_stream (FILE * file)
{
    image *vimage;
    struct jpeg_decompress_struct cinfo;
    unsigned char *line[16], *ptr;
    int x, y, i, w, h, ofs;
    int maxbuf;
    struct jpeg_error_mgr pub;

    cinfo.err = jpeg_std_error (&pub);
    jpeg_create_decompress (&cinfo);
    pub.error_exit = g_JPEGFatalErrorHandler;

    /* error handler to longjmp to, we want to preserve signals */
    if (setjmp (setjmp_buffer)) {
		jpeg_abort_decompress (&cinfo);
		fclose (file);
		return NULL;
    }

    jpeg_stdio_src (&cinfo, file);
    jpeg_read_header (&cinfo, TRUE);
    cinfo.do_fancy_upsampling = FALSE;
    cinfo.do_block_smoothing = FALSE;
    jpeg_start_decompress (&cinfo);
    w = cinfo.output_width;
    h = cinfo.output_height;
    vimage = image_new (w, h);
    if (!vimage) {
		jpeg_destroy_decompress (&cinfo);
		return NULL;
    }

    if (cinfo.rec_outbuf_height > 16) {
		fprintf (stderr,
			 "ERROR image_from_jpeg : (image_from_jpeg.c) JPEG uses line buffers > 16. Cannot load.\n");
		image_free (vimage);
		return NULL;
    }

    maxbuf = vimage->width * vimage->height;

	if (cinfo.output_components == 3) {
		ofs = 0;
		if ((ptr = (char *) malloc (w * 3 * cinfo.rec_outbuf_height)) == NULL) {
			perror ("malloc failed ");
			exit (1);
		}

		for (y = 0; y < h; y += cinfo.rec_outbuf_height) {
			for (i = 0; i < cinfo.rec_outbuf_height; i++)
				line[i] = ptr + (w * 3 * i);

			jpeg_read_scanlines (&cinfo, line, cinfo.rec_outbuf_height);
			for (x = 0; x < w * cinfo.rec_outbuf_height; x++) {
				if (ofs < maxbuf) {
					vimage->buf[ofs] =
					COL (ptr[x + x + x], ptr[x + x + x + 1],
						 ptr[x + x + x + 2]);
					ofs++;
				}
			}
		}

		free (ptr);
	} else if (cinfo.output_components == 1) {
		ofs = 0;
		for (i = 0; i < cinfo.rec_outbuf_height; i++) {
			if ((line[i] = (unsigned char *) malloc (w)) == NULL) {
				int t = 0;

				for (t = 0; t < i; t++)
					free (line[t]);
				jpeg_destroy_decompress (&cinfo);
				image_free (vimage);
				return NULL;
			}
		}

		for (y = 0; y < h; y += cinfo.rec_outbuf_height) {
			jpeg_read_scanlines (&cinfo, line, cinfo.rec_outbuf_height);
			for (i = 0; i < cinfo.rec_outbuf_height; i++) {
				for (x = 0; x < w; x++) {
					vimage->buf[ofs++] = COL (line[i][x], line[i][x], line[i][x]);
				}
			}
		}

		for (i = 0; i < cinfo.rec_outbuf_height; i++)
			free (line[i]);
	}

    jpeg_finish_decompress (&cinfo);
    jpeg_destroy_decompress (&cinfo);

    return vimage;
}

/* Sauvegarde l'image pimage dans le fichier file      */
/* sous le format JPEG. Return -1 = si ERREUR, 0 sinon */
int
image_save_to_jpeg (image * pimage, char *file, int jpeg_quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    FILE *f;
    char *data;
    int i, x;

    f = fopen (file, "w");
	if (f) {
		cinfo.err = jpeg_std_error (&jerr);
		jpeg_create_compress (&cinfo);
		jerr.error_exit = g_JPEGFatalErrorHandler;

		/* error handler to longjmp to, we want to preserve signals */
		if (setjmp (setjmp_buffer)) {
			jpeg_destroy_compress (&cinfo);
			fclose (f);
			return -1;
		}

		jpeg_stdio_dest (&cinfo, f);
		cinfo.image_width = pimage->width;
		cinfo.image_height = pimage->height;
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
		jpeg_set_defaults (&cinfo);
		jpeg_set_quality (&cinfo, (jpeg_quality * JPEG_QUALITY) >> 8, TRUE);
		jpeg_start_compress (&cinfo, TRUE);

		row_stride = cinfo.image_width * 3;
		if ((data = (char *) malloc (row_stride)) == NULL) {
			perror ("malloc failed ");
			exit (1);
		}

		i = 0;
		while (cinfo.next_scanline < cinfo.image_height) {
			for (x = 0; x < pimage->width; x++) {
				data[x + x + x] = COL_RED (pimage->buf[i]);
				data[x + x + x + 1] = COL_GREEN (pimage->buf[i]);
				data[x + x + x + 2] = COL_BLUE (pimage->buf[i]);
				i++;
			}

			row_pointer[0] = data;
			/*im->rgb_data + (cinfo.next_scanline * row_stride); */
			jpeg_write_scanlines (&cinfo, row_pointer, 1);
		}

		jpeg_finish_compress (&cinfo);

		free (data);
		fclose (f);
		return 0;
	}

    return -1;
}

/* Sauvegarde l'image pimage dans le flux stream       */
/* sous le format JPEG. Return -1 = si ERREUR, 0 sinon */
int
image_save_to_jpeg_stream (image * pimage, FILE * stream, int jpeg_quality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    int row_stride;
    char *data;
    int i, x;

    cinfo.err = jpeg_std_error (&jerr);
    jpeg_create_compress (&cinfo);

    jerr.error_exit = g_JPEGFatalErrorHandler;

    /* error handler to longjmp to, we want to preserve signals */
    if (setjmp (setjmp_buffer)) {
		jpeg_destroy_compress (&cinfo);
		return -1;
    }

    jpeg_stdio_dest (&cinfo, stream);
    cinfo.image_width = pimage->width;
    cinfo.image_height = pimage->height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults (&cinfo);
    jpeg_set_quality (&cinfo, (jpeg_quality * JPEG_QUALITY) >> 8, TRUE);
    jpeg_start_compress (&cinfo, TRUE);
    row_stride = cinfo.image_width * 3;
    if ((data = (char *) malloc (row_stride)) == NULL) {
		perror ("malloc failed ");
		exit (1);
    }

    i = 0;
	while (cinfo.next_scanline < cinfo.image_height) {
		for (x = 0; x < pimage->width; x++) {
			data[x + x + x] = COL_RED (pimage->buf[i]);
			data[x + x + x + 1] = COL_GREEN (pimage->buf[i]);
			data[x + x + x + 2] = COL_BLUE (pimage->buf[i]);
			i++;
		}

		row_pointer[0] = data;
		/*im->rgb_data + (cinfo.next_scanline * row_stride); */
		jpeg_write_scanlines (&cinfo, row_pointer, 1);
	}

    jpeg_finish_compress (&cinfo);
    free (data);
    return 0;
}

/* cree une nouvelle image */
image *
image_new (int32 width, int32 height)
{
    image *vimage;

    if ((vimage = (image *) malloc (sizeof (image))) == NULL) {
		perror ("malloc failed ");
		exit (1);
    }

    vimage->width = width;
    vimage->height = height;

    if ((vimage->buf = (pix *) malloc (width * height * sizeof (pix))) == NULL) {
		perror ("malloc failed ");
		exit (1);
    }

    return (vimage);
}

/* libere une image */
void
image_free (image * pimage)
{
    /* libere la buffer qui contient les points de l'image */
    free (pimage->buf);

    /* libere la structure qui definie l'image */
    free (pimage);
}

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest en prenant        */
/* toujours le point le plus proche.               */
void
image_resize_nearest (image * pdest, image * psrc, int32 width, int32 height)
{
    int32 vx, vy;
    pix vcol;
#ifdef USE_INTEGER
    int32 rx, ry;

	for (vy = 0; vy < height; vy++) {
		for (vx = 0; vx < width; vx++) {
			rx = ((vx * psrc->width) / width);
			ry = ((vy * psrc->height) / height);
#else // USE_INTEGER
    float rx, ry;
    float width_scale, height_scale;

    width_scale = (float) psrc->width / (float) width;
    height_scale = (float) psrc->height / (float) height;

	for (vy = 0; vy < height; vy++) {
		for (vx = 0; vx < width; vx++) {
			rx = vx * width_scale;
			ry = vy * height_scale;
#endif // USE_INTEGER
			vcol = get_pix (psrc, (int32) rx, (int32) ry);
			put_pix_alpha_replace (pdest, vx, vy, vcol);
		}
	}
}


/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest en fusionnant les */
/* points. L'image résultat doit être plus petite  */
/* que l'image d'origine.                          */
void
image_downsize (image * pdest, image * psrc, int32 width, int32 height)
{
    int32 vx, vy;
    pix vcol;
    float rx, ry;
    float width_scale, height_scale;
    float red, green, blue, alpha;
    int i, j;
    int32 half_square_width, half_square_height;
    float round_width, round_height;

    if ((pdest == NULL) || (psrc == NULL) || (psrc->width < width) ||
		(psrc->height < height))
	return;

    width_scale = (float) psrc->width / (float) width;
    height_scale = (float) psrc->height / (float) height;

    half_square_width = (int32) (width_scale / 2.0);
    half_square_height = (int32) (height_scale / 2.0);
    round_width = (width_scale / 2.0) - (float) half_square_width;
    round_height = (height_scale / 2.0) - (float) half_square_height;
    if (round_width > 0.0)
		half_square_width++;
    else
		round_width = 1.0;

    if (round_height > 0.0)
		half_square_height++;
    else
		round_height = 1.0;

	for (vy = 0; vy < height; vy++) {
		for (vx = 0; vx < width; vx++) {
			rx = vx * width_scale;
			ry = vy * height_scale;
			vcol = get_pix (psrc, (int32) rx, (int32) ry);

			red = green = blue = alpha = 0.0;

			for (j = 0; j < half_square_height * 2; j++) {
				for (i = 0; i < half_square_width * 2; i++) {
					vcol =
						get_pix(psrc, ((int32) rx) - half_square_width + i,
								 ((int32) ry) - half_square_height + j);

					if (((j == 0) || (j == (half_square_height * 2) - 1)) &&
						((i == 0) || (i == (half_square_width * 2) - 1))) {
						red +=
							round_width * round_height * (float) COL_RED (vcol);
						green +=
							round_width * round_height * (float) COL_GREEN (vcol);
						blue +=
							round_width * round_height * (float) COL_BLUE (vcol);
						alpha +=
							round_width * round_height * (float) COL_ALPHA (vcol);
					} else if ((j == 0) || (j == (half_square_height * 2) - 1)){
						red += round_height * (float) COL_RED (vcol);
						green += round_height * (float) COL_GREEN (vcol);
						blue += round_height * (float) COL_BLUE (vcol);
						alpha += round_height * (float) COL_ALPHA (vcol);
					} else if ((i == 0) || (i == (half_square_width * 2) - 1)) {
						red += round_width * (float) COL_RED (vcol);
						green += round_width * (float) COL_GREEN (vcol);
						blue += round_width * (float) COL_BLUE (vcol);
						alpha += round_width * (float) COL_ALPHA (vcol);
					} else {
						red += (float) COL_RED (vcol);
						green += (float) COL_GREEN (vcol);
						blue += (float) COL_BLUE (vcol);
						alpha += (float) COL_ALPHA (vcol);
					}
				}
			}

			red /= width_scale * height_scale;
			green /= width_scale * height_scale;
			blue /= width_scale * height_scale;
			alpha /= width_scale * height_scale;

			/* on sature les valeurs */
			red = (red > 255.0) ? 255.0 : ((red < 0.0) ? 0.0 : red);
			green = (green > 255.0) ? 255.0 : ((green < 0.0) ? 0.0 : green);
			blue = (blue > 255.0) ? 255.0 : ((blue < 0.0) ? 0.0 : blue);
			alpha = (alpha > 255.0) ? 255.0 : ((alpha < 0.0) ? 0.0 : alpha);

			put_pix_alpha_replace (pdest, vx, vy,
					   COL_FULL ((uint8) red, (uint8) green,
							 (uint8) blue, (uint8) alpha));
		}
	}
}

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest. On determine les */
/* points par interpolation bilinaire.             */
void
image_resize_bilinear (image * pdest, image * psrc, int32 width, int32 height)
{
    int32 vx, vy;
    pix vcol, vcol1, vcol2, vcol3, vcol4;
    float rx, ry;
    float width_scale, height_scale;
    float x_dist, y_dist;

    width_scale = (float) psrc->width / (float) width;
    height_scale = (float) psrc->height / (float) height;

	for (vy = 0; vy < height; vy++) {
		for (vx = 0; vx < width; vx++) {
			rx = vx * width_scale;
			ry = vy * height_scale;
			vcol1 = get_pix (psrc, (int32) rx, (int32) ry);
			vcol2 = get_pix (psrc, ((int32) rx) + 1, (int32) ry);

			vcol3 = get_pix (psrc, (int32) rx, ((int32) ry) + 1);
			vcol4 = get_pix (psrc, ((int32) rx) + 1, ((int32) ry) + 1);

			x_dist = rx - ((float) ((int32) rx));
			y_dist = ry - ((float) ((int32) ry));
			vcol = COL_FULL ((uint8) ((COL_RED (vcol1) * (1.0 - x_dist)
						   + COL_RED (vcol2) * (x_dist)) * (1.0 - y_dist)
						  + (COL_RED (vcol3) * (1.0 - x_dist) +
						 COL_RED (vcol4) * (x_dist)) * (y_dist)),
					 (uint8) ((COL_GREEN (vcol1) * (1.0 - x_dist) +
						   COL_GREEN (vcol2) * (x_dist)) * (1.0 - y_dist)
						  + (COL_GREEN (vcol3) * (1.0 - x_dist) +
						 COL_GREEN (vcol4) * (x_dist)) * (y_dist)),
					 (uint8) ((COL_BLUE (vcol1) * (1.0 - x_dist)
						   + COL_BLUE (vcol2) * (x_dist)) * (1.0 - y_dist)
						  + (COL_BLUE (vcol3) * (1.0 - x_dist) +
						 COL_BLUE (vcol4) * (x_dist)) * (y_dist)),
					 (uint8) ((COL_ALPHA (vcol1) * (1.0 - x_dist) +
						   COL_ALPHA (vcol2) * (x_dist)) * (1.0 - y_dist)
						  + (COL_ALPHA (vcol3) * (1.0 - x_dist) +
						 COL_ALPHA (vcol4) * (x_dist)) * (y_dist))
			);

			put_pix_alpha_replace (pdest, vx, vy, vcol);
		}
	}
}

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest. On determine les */
/* points par interpolation avec une DCT 2D des 64 */
/* points les plus pres.                           */
void
image_resize_dct (image * pdest, image * psrc, int32 width, int32 height)
{
    float dct_coeff[8][8];	/* 8 fonctions de bases definie par 8 valeurs */
    uint8 val[8][8];
    float dct_res[8][8];	/* coefficient 3d qui correspondent a nos pixels */
    float dc_coeff;

    /* initialise le tableau des fonctions de bases */
    double f1, f2, tmp;
    int x, y, i1, i2;
    boolean do_dct = TRUE;
    int32 vx, vy;
    float rx, ry, last_rx, last_ry;
    float width_scale, height_scale;
    float x_dist, y_dist;
    pix vcol = BLACK;
    uint32 nb_pix;
    float percent_done, percent_tmp;

    void init_dct (void)
	{
		f1 = 2.0 * atan (1.0) / 8.0;
		for (y = 0; y < 8; y++) {
			f2 = (double) y *f1;
			for (x = 0; x < 8; x++) {
				tmp = cos ((double) (2 * x + 1) * f2);
				if (y == 0)
					dct_coeff[y][x] = (float) (1.0 / sqrt (2.0));
				else
					dct_coeff[y][x] = (float) tmp;
			}
		}

		dc_coeff = 1.0 / sqrt (2.0);
	}

    void calcul_dct (void)
	{
		for (i2 = 0; i2 < 8; i2++) {
			for (i1 = 0; i1 < 8; i1++) {
				dct_res[i2][i1] = 0.0;

				for (y = 0; y < 8; y++)
					for (x = 0; x < 8; x++)
					dct_res[i2][i1] += dct_coeff[i1][x] * dct_coeff[i2][y]
						* (float) val[y][x];

				dct_res[i2][i1] /= 16.0;
			}
		}
	}

    uint8 calcul_pix (float px, float py)
	{
		uint8 res;
		float temp;
		float dct_coeff_hor[8], dct_coeff_ver[8];

		for (y = 0; y < 8; y++) {
			f2 = (double) y *f1;
			if (y == 0) {
				dct_coeff_hor[y] = dc_coeff;
			} else {
				tmp = cos ((double) (2.0 * (4.0 - px) + 1.0) * f2);
				dct_coeff_hor[y] = (float) tmp;
			}

			if (y == 0) {
				dct_coeff_ver[y] = dc_coeff;
			} else {
				tmp = cos ((double) (2.0 * (4.0 - py) + 1.0) * f2);
				dct_coeff_ver[y] = (float) tmp;
			}
		}

		temp = 0.0;
		for (y = 0; y < 8; y++)
			for (x = 0; x < 8; x++)
			temp += dct_coeff_hor[x] * dct_coeff_ver[y] * dct_res[y][x];

		if (temp > 255.0)
			res = 255;
		else if (temp < 0.0)
			res = 0;
		else
			res = (uint8) temp;
		return (res);
	}

    /* corp de la procedure */
    init_dct ();

    width_scale = (float) psrc->width / (float) width;
    height_scale = (float) psrc->height / (float) height;

    nb_pix = 0;
    percent_done = 0.0;

    last_rx = last_ry = -1.0;

	for (vy = 0; vy < height; vy++) {
		for (vx = 0; vx < width; vx++) {
			rx = vx * width_scale;
			ry = vy * height_scale;

			x_dist = rx - ((float) ((int32) rx));
			y_dist = ry - ((float) ((int32) ry));

			if ((((int32) last_rx) != ((int32) rx))
					|| (((int32) last_ry) != ((int32) ry)))
				do_dct = TRUE;
			else
				do_dct = FALSE;

			last_rx = rx;
			last_ry = ry;

			/* pour le rouge */
			for (y = 0; y < 8; y++)
				for (x = 0; x < 8; x++)
					val[y][x] = COL_RED (get_pix (psrc, ((int32) rx) - x + 4,
												  ((int32) ry) - y + 4));

			calcul_dct();
			SET_COL_RED (vcol, calcul_pix (x_dist, y_dist));

			/* pour le vert */
			for (y = 0; y < 8; y++)
				for (x = 0; x < 8; x++)
					val[y][x] = COL_GREEN (get_pix (psrc, ((int32) rx) - x + 4,
														((int32) ry) - y + 4));

			calcul_dct();
			SET_COL_GREEN (vcol, calcul_pix (x_dist, y_dist));

			/* pour le bleu */
			for (y = 0; y < 8; y++)
				for (x = 0; x < 8; x++)
					val[y][x] = COL_BLUE (get_pix (psrc, ((int32) rx) - x + 4,
												   ((int32) ry) - y + 4));

			calcul_dct();
			SET_COL_BLUE (vcol, calcul_pix (x_dist, y_dist));

			/* on place le pixel calcule */
			put_pix_alpha_replace (pdest, vx, vy, vcol);

			nb_pix++;
			percent_tmp = (float) nb_pix / (float) (width * height);
			if (percent_tmp - percent_done >= 0.01) {
				percent_done =
					(float) (((uint32) (percent_tmp * 100.0)) / 100.0);
				fprintf (stdout, "\rpercent done = %3.0f%c",
					 percent_done * 100.0, '%');
				fflush (stdout);
			}
		}
	}

    fprintf (stdout, "\n");
}

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest. On determine les */
/* points par interpolation avec une BSpline de    */
/* Catmull-Rom cubique.                            */
void
image_resize_catmull_rom (image * pdest, image * psrc, int32 width,
			  int32 height)
{
    /* definie la matrice des BSplines de Catmull-Rom */
    float mat[16] = { -1.0, 3.0, -3.0, 1.0,
						2.0, -5.0, 4.0, -1.0,
						-1.0, 0.0, 1.0, 0.0,
						0.0, 2.0, 0.0, 0.0
    };
    /* tableau des points de contrainte */
    float dot[12];
    float x, y, z;
    pix vcol = BLACK;
    pix vcol1, vcol2, vcol3, vcol4;
    int vy, vx;
    float rx = 0.0, ry;
    float width_scale, height_scale;
    image *img_width;

    /* calcul la valeur du point a la position t */
    void calcul_dot (float t)
	{
		float t2, t3;
		float xt, yt, zt, wt;

		/* calcul t au carre */
		t2 = t * t;

		/* calcul t au cube */
		t3 = t2 * t;

		/* on multiplie le vecteur (t3,t2,t,1) par la matrice mat */
		xt = t3 * mat[0] + t2 * mat[4] + t * mat[8] + mat[12];
		yt = t3 * mat[1] + t2 * mat[5] + t * mat[9] + mat[13];
		zt = t3 * mat[2] + t2 * mat[6] + t * mat[10] + mat[14];
		wt = t3 * mat[3] + t2 * mat[7] + t * mat[11] + mat[15];

		/* on multiplie le resultat par la matrice des contraintes */
		x = xt * dot[0] + yt * dot[3] + zt * dot[6] + wt * dot[9];
		y = xt * dot[1] + yt * dot[4] + zt * dot[7] + wt * dot[10];
		z = xt * dot[2] + yt * dot[5] + zt * dot[8] + wt * dot[11];
	}

    width_scale = (float) psrc->width / (float) width;
    height_scale = (float) psrc->height / (float) height;

    img_width = image_new (width, psrc->height);

    /* pour le moment, on redimentionne seulement en x */
	for (vy = 0; vy < psrc->height; vy++) {
		for (vx = 0; vx < width; vx++) {

			rx = vx * width_scale;
			ry = vy * height_scale;

			vcol1 = get_pix (psrc, ((int32) rx) - 1, vy);
			vcol2 = get_pix (psrc, ((int32) rx), vy);
			vcol3 = get_pix (psrc, ((int32) rx) + 1, vy);
			vcol4 = get_pix (psrc, ((int32) rx) + 2, vy);

			/* on fait le calcul pour le rouge */
			dot[0] = 1.0;
			dot[1] = (float) COL_RED (vcol1);
			dot[2] = 1.0;
			dot[3] = 2.0;
			dot[4] = (float) COL_RED (vcol2);
			dot[5] = 1.0;
			dot[6] = 3.0;
			dot[7] = (float) COL_RED (vcol3);
			dot[8] = 1.0;
			dot[9] = 4.0;
			dot[10] = (float) COL_RED (vcol4);
			dot[11] = 1.0;

			/* calcul le point */
			calcul_dot (rx - ((float) ((int32) rx)));

			y /= 2.0;
			y = MIN (y, 255);
			y = MAX (y, 0);
			SET_COL_RED (vcol, (uint8) y);

			/* on fait le calcul pour le vert */
			dot[0] = 1.0;
			dot[1] = (float) COL_GREEN (vcol1);
			dot[2] = 1.0;
			dot[3] = 2.0;
			dot[4] = (float) COL_GREEN (vcol2);
			dot[5] = 1.0;
			dot[6] = 3.0;
			dot[7] = (float) COL_GREEN (vcol3);
			dot[8] = 1.0;
			dot[9] = 4.0;
			dot[10] = (float) COL_GREEN (vcol4);
			dot[11] = 1.0;

			/* calcul le point */
			calcul_dot (rx - ((float) ((int32) rx)));

			y /= 2.0;
			y = MIN (y, 255);
			y = MAX (y, 0);
			SET_COL_GREEN (vcol, (uint8) y);

			/* on fait le calcul pour le bleu */
			dot[0] = 1.0;
			dot[1] = (float) COL_BLUE (vcol1);
			dot[2] = 1.0;
			dot[3] = 2.0;
			dot[4] = (float) COL_BLUE (vcol2);
			dot[5] = 1.0;
			dot[6] = 3.0;
			dot[7] = (float) COL_BLUE (vcol3);
			dot[8] = 1.0;
			dot[9] = 4.0;
			dot[10] = (float) COL_BLUE (vcol4);
			dot[11] = 1.0;

			/* calcul le point */
			calcul_dot (rx - ((float) ((int32) rx)));

			y /= 2.0;
			y = MIN (y, 255);
			y = MAX (y, 0);
			SET_COL_BLUE (vcol, (uint8) y);

			SET_COL_ALPHA (vcol, 0xFF);
			put_pix_alpha_replace (img_width, vx, vy, vcol);
		}
	}

    /* on redimentionne seulement en y */
	for (vx = 0; vx < width; vx++) {
		for (vy = 0; vy < height; vy++) {
			ry = vy * height_scale;

			vcol1 = get_pix (img_width, vx, ((int32) ry) - 1);
			vcol2 = get_pix (img_width, vx, ((int32) ry));
			vcol3 = get_pix (img_width, vx, ((int32) ry) + 1);
			vcol4 = get_pix (img_width, vx, ((int32) ry) + 2);

			/* on fait le calcul pour le rouge */
			dot[0] = 1.0;
			dot[1] = (float) COL_RED (vcol1);
			dot[2] = 1.0;
			dot[3] = 2.0;
			dot[4] = (float) COL_RED (vcol2);
			dot[5] = 1.0;
			dot[6] = 3.0;
			dot[7] = (float) COL_RED (vcol3);
			dot[8] = 1.0;
			dot[9] = 4.0;
			dot[10] = (float) COL_RED (vcol4);
			dot[11] = 1.0;

			/* calcul le point */
			calcul_dot (ry - ((float) ((int32) ry)));

			y /= 2.0;
			y = MIN (y, 255);
			y = MAX (y, 0);
			SET_COL_RED (vcol, (uint8) y);

			/* on fait le calcul pour le vert */
			dot[0] = 1.0;
			dot[1] = (float) COL_GREEN (vcol1);
			dot[2] = 1.0;
			dot[3] = 2.0;
			dot[4] = (float) COL_GREEN (vcol2);
			dot[5] = 1.0;
			dot[6] = 3.0;
			dot[7] = (float) COL_GREEN (vcol3);
			dot[8] = 1.0;
			dot[9] = 4.0;
			dot[10] = (float) COL_GREEN (vcol4);
			dot[11] = 1.0;

			/* calcul le point */
			calcul_dot (rx - ((float) ((int32) rx)));

			y /= 2.0;
			y = MIN (y, 255);
			y = MAX (y, 0);
			SET_COL_GREEN (vcol, (uint8) y);

			/* on fait le calcul pour le bleu */
			dot[0] = 1.0;
			dot[1] = (float) COL_BLUE (vcol1);
			dot[2] = 1.0;
			dot[3] = 2.0;
			dot[4] = (float) COL_BLUE (vcol2);
			dot[5] = 1.0;
			dot[6] = 3.0;
			dot[7] = (float) COL_BLUE (vcol3);
			dot[8] = 1.0;
			dot[9] = 4.0;
			dot[10] = (float) COL_BLUE (vcol4);
			dot[11] = 1.0;

			/* calcul le point */
			calcul_dot (rx - ((float) ((int32) rx)));

			y /= 2.0;
			y = MIN (y, 255);
			y = MAX (y, 0);
			SET_COL_BLUE (vcol, (uint8) y);

			SET_COL_ALPHA (vcol, 0xFF);
			put_pix_alpha_replace (pdest, vx, vy, vcol);
		}
	}

    image_free (img_width);
}

/* Redimentionne l'image psrc et place le resultat          */
/* dans pdest. pdest doit être 2 fois plus grande que psrc. */
void
image_resize_tv (image * pdest, image * psrc)
{
    pix vcol, vcol1, vcol2;
    int32 vx, vy;

    /* si les préconditions ne sont pas remplies, on quitte */
    if ((pdest == NULL) || (psrc == NULL) ||
			(pdest->width != psrc->width * 2)
			|| (pdest->height != psrc->height * 2))
		return;

	for (vy = 0; vy < psrc->height; vy++) {
		for (vx = 0; vx < psrc->width; vx++) {
			vcol = get_pix (psrc, vx, vy);
			vcol2 = get_pix (psrc, vx + 1, vy);

			put_pix_alpha_replace (pdest, (vx << 1), vy << 1, vcol);

			vcol = COL((uint8) ((((float) COL_RED (vcol)) * 0.5) +
							 (((float) COL_RED (vcol2)) * 0.25)),
						(uint8) ((((float) COL_GREEN (vcol)) * 0.5) +
							 (((float) COL_GREEN (vcol2)) * 0.25)),
						(uint8) ((((float) COL_BLUE (vcol)) * 0.5) +
							 (((float) COL_BLUE (vcol2)) * 0.25)));

			put_pix_alpha_replace (pdest, (vx << 1) + 1, vy << 1, vcol);
		}
	}

	for (vy = 0; vy < psrc->height; vy++) {
		for (vx = 0; vx < psrc->width; vx++) {
			vcol1 = get_pix (psrc, vx, vy);
			vcol2 = get_pix (psrc, vx, vy + 1);

			vcol = COL ((uint8)
				((((float) COL_RED (vcol1)) * 0.5) +
					 (((float) COL_RED (vcol2)) * 0.25)),
				(uint8) ((((float) COL_GREEN (vcol1)) * 0.5) +
					 (((float) COL_GREEN (vcol2)) * 0.25)),
				(uint8) ((((float) COL_BLUE (vcol1)) * 0.5) +
					 (((float) COL_BLUE (vcol2)) * 0.25)));

			put_pix_alpha_replace (pdest, (vx << 1), (vy << 1) + 1, vcol);
			put_pix_alpha_replace (pdest, (vx << 1) + 1, (vy << 1) + 1, vcol);
		}
	}
}

/* Redimentionne l'image psrc et place le resultat          */
/* dans pdest. pdest doit être 2 fois plus grande que psrc. */
void
image_resize_tv2 (image * pdest, image * psrc)
{
    pix vcol, vcol1, vcol2, vcol3, vcol4;
    int32 vx, vy;

    void redim_col (float dim)
	{
		vcol = COL ((uint8)
				MIN (255, MAX (0, (((float) COL_RED (vcol)) * dim))),
				(uint8) MIN (255, MAX (0, (((float) COL_GREEN (vcol)) * dim))),
				(uint8) MIN (255, MAX (0, (((float) COL_BLUE (vcol)) * dim))));
	}

    void redim_lum (float dim_y, float dim_cr, float dim_cb)
	{
		float cr, cb, y;
		float r, g, b;

		y = 0.299 * (float) COL_RED (vcol) +
			0.587 * (float) COL_GREEN (vcol) +
			0.114 * (float) COL_BLUE (vcol);

		cr = (float) COL_RED (vcol) - y;
		cb = (float) COL_BLUE (vcol) - y;

		/*    y = (1.0+cos(M_PI+((y/255.0)*M_PI)))*128.0;*/

		y *= dim_y;
		cr *= dim_cr;
		cb *= dim_cb;

		r = cr + y;
		b = cb + y;
		g = y * 1.7 - r * 0.509 - b * 0.194;

		r = MIN (255.0, MAX (0, r));
		g = MIN (255.0, MAX (0, g));
		b = MIN (255.0, MAX (0, b));

		vcol = COL ((uint8) r, (uint8) g, (uint8) b);
	}

    /* si les préconditions ne sont pas remplies, on quitte */
    if ((pdest == NULL) || (psrc == NULL) ||
			(pdest->width != psrc->width * 2)
			|| (pdest->height != psrc->height * 2))
		return;

	for (vy = 0; vy < psrc->height; vy++) {
		for (vx = 0; vx < psrc->width; vx++) {
			vcol1 = get_pix (psrc, vx, vy);
			vcol2 = get_pix (psrc, vx + 1, vy);
			vcol3 = get_pix (psrc, vx, vy + 1);
			vcol4 = get_pix (psrc, vx + 1, vy + 1);

			vcol = vcol1;
			redim_lum (1.5, 1.7, 1.7);
			put_pix_alpha_replace (pdest, (vx << 1), (vy << 1), vcol);
			vcol = COL ((uint8)
				((((float) COL_RED (vcol1)) * 0.3) +
				 (((float) COL_RED (vcol2)) * 0.6)),
				(uint8) ((((float) COL_GREEN (vcol1)) * 0.3) +
					 (((float) COL_GREEN (vcol2)) * 0.6)),
				(uint8) ((((float) COL_BLUE (vcol1)) * 0.3) +
					 (((float) COL_BLUE (vcol2)) * 0.6)));

	/*      redim_col(0.95);*/
			redim_lum (1.3, 1.7, 1.7);
			put_pix_alpha_replace (pdest, (vx << 1) + 1, (vy << 1), vcol);
			vcol = COL ((uint8)
				((((float) COL_RED (vcol1)) * 0.3) +
				 (((float) COL_RED (vcol3)) * 0.6)),
				(uint8) ((((float) COL_GREEN (vcol1)) * 0.3) +
					 (((float) COL_GREEN (vcol3)) * 0.6)),
				(uint8) ((((float) COL_BLUE (vcol1)) * 0.3) +
					 (((float) COL_BLUE (vcol3)) * 0.6)));

	/*      redim_col(0.8);*/
			redim_lum (1.0, 1.7, 1.7);
			put_pix_alpha_replace (pdest, (vx << 1), (vy << 1) + 1, vcol);
			vcol = COL ((uint8)
				((((float) COL_RED (vcol1)) * 0.11) +
				 (((float) COL_RED (vcol2)) * 0.2) +
				 (((float) COL_RED (vcol3)) * 0.2) +
				 (((float) COL_RED (vcol4)) * 0.44)),
				(uint8) ((((float) COL_GREEN (vcol1)) * 0.11) +
					 (((float) COL_GREEN (vcol2)) * 0.2) +
					 (((float) COL_GREEN (vcol3)) * 0.2) +
					 (((float) COL_GREEN (vcol4)) * 0.44)),
				(uint8) ((((float) COL_BLUE (vcol1)) * 0.11) +
					 (((float) COL_BLUE (vcol2)) * 0.2) +
					 (((float) COL_BLUE (vcol3)) * 0.2) +
					 (((float) COL_BLUE (vcol4)) * 0.44)));
			redim_col (0.7);
			put_pix_alpha_replace (pdest, (vx << 1) + 1, (vy << 1) + 1, vcol);
		}
	}
}

/* Redimentionne l'image psrc et place le resultat          */
/* dans pdest. pdest doit être 2 fois plus grande que psrc. */
void
image_resize_most (image * pdest, image * psrc)
{
    pix vcol1, vcol2, vcol3, vcol4, vcolm;
    int32 vx, vy;

    pix find_most_middle (void)
	{
		float y[4];
		int pos[4];
		int min_pos, i1, i2;
		float swap_float;
		int swap_int;

		y[0] = (0.299 * (float) COL_RED (vcol1) +
			0.587 * (float) COL_GREEN (vcol1) +
			0.114 * (float) COL_BLUE (vcol1)) * 1.2;
		y[1] = 0.299 * (float) COL_RED (vcol2) +
			0.587 * (float) COL_GREEN (vcol2) +
			0.114 * (float) COL_BLUE (vcol2);
		y[2] = 0.299 * (float) COL_RED (vcol3) +
			0.587 * (float) COL_GREEN (vcol3) +
			0.114 * (float) COL_BLUE (vcol3);
		y[3] = 0.299 * (float) COL_RED (vcol4) +
			0.587 * (float) COL_GREEN (vcol4) +
			0.114 * (float) COL_BLUE (vcol4);
		pos[0] = 0;
		pos[1] = 1;
		pos[2] = 2;
		pos[3] = 3;

		for (i1 = 0; i1 < 4 - 1; i1++) {
			min_pos = i1;
			for (i2 = i1; i2 < 4; i2++) {
			if (y[i2] < y[min_pos])
				min_pos = i2;
			}

			swap_float = y[i1];
			swap_int = pos[i1];
			y[i1] = y[min_pos];
			pos[i1] = pos[min_pos];
			y[min_pos] = swap_float;
			pos[min_pos] = swap_int;
		}

		switch (pos[1]) {
		  case 0:
			return (vcol1);
		  case 1:
			return (vcol2);
		  case 2:
			return (vcol3);
		  case 3:
			return (vcol4);
		}

		return (BLACK);
	}

    pix find_most_upper (void)
	{
		float y[3];
		int pos[3];
		int min_pos, i1, i2;
		float swap_float;
		int swap_int;

		y[0] = 0.299 * (float) COL_RED (vcol1) +
			0.587 * (float) COL_GREEN (vcol1) +
			0.114 * (float) COL_BLUE (vcol1);
		y[1] = 0.299 * (float) COL_RED (vcol2) +
			0.587 * (float) COL_GREEN (vcol2) +
			0.114 * (float) COL_BLUE (vcol2);
		y[2] = (0.299 * (float) COL_RED (vcolm) +
			0.587 * (float) COL_GREEN (vcolm) +
			0.114 * (float) COL_BLUE (vcolm)) * 1.2;

		pos[0] = 0;
		pos[1] = 1;
		pos[2] = 2;

		for (i1 = 0; i1 < 3 - 1; i1++) {
			min_pos = i1;
			for (i2 = i1; i2 < 3; i2++) {
				if (y[i2] < y[min_pos])
					min_pos = i2;
			}
			swap_float = y[i1];
			swap_int = pos[i1];
			y[i1] = y[min_pos];
			pos[i1] = pos[min_pos];
			y[min_pos] = swap_float;
			pos[min_pos] = swap_int;
		}

		switch (pos[1]) {
		  case 0:
			return (vcol1);
		  case 1:
			return (vcol2);
		  case 2:
			return (vcolm);
		}

		return (vcolm);
	}

    pix find_most_left (void)
	{
		float y[3];
		int pos[3];
		int min_pos, i1, i2;
		float swap_float;
		int swap_int;

		y[0] = 0.299 * (float) COL_RED (vcol1) +
			0.587 * (float) COL_GREEN (vcol1) +
			0.114 * (float) COL_BLUE (vcol1);
		y[1] = 0.299 * (float) COL_RED (vcol3) +
			0.587 * (float) COL_GREEN (vcol3) +
			0.114 * (float) COL_BLUE (vcol3);
		y[2] = (0.299 * (float) COL_RED (vcolm) +
			0.587 * (float) COL_GREEN (vcolm) +
			0.114 * (float) COL_BLUE (vcolm)) * 1.2;

		pos[0] = 0;
		pos[1] = 1;
		pos[2] = 2;

		for (i1 = 0; i1 < 3 - 1; i1++) {
			min_pos = i1;
			for (i2 = i1; i2 < 3; i2++) {
				if (y[i2] < y[min_pos])
					min_pos = i2;
			}
			swap_float = y[i1];
			swap_int = pos[i1];
			y[i1] = y[min_pos];
			pos[i1] = pos[min_pos];
			y[min_pos] = swap_float;
			pos[min_pos] = swap_int;
		}

		switch (pos[1]) {
		  case 0:
			return (vcol1);
		  case 1:
			return (vcol3);
		  case 2:
			return (vcolm);
		}

		return (vcolm);
	}

    /* si les préconditions ne sont pas remplies, on quitte */
    if ((pdest == NULL) || (psrc == NULL) ||
			(pdest->width != psrc->width * 2)
			|| (pdest->height != psrc->height * 2))
		return;

	for (vy = 0; vy < psrc->height; vy++) {
		for (vx = 0; vx < psrc->width; vx++) {
			vcol1 = get_pix (psrc, vx, vy);
			vcol2 = get_pix (psrc, vx + 1, vy);
			vcol3 = get_pix (psrc, vx, vy + 1);
			vcol4 = get_pix (psrc, vx + 1, vy + 1);

			put_pix_alpha_replace (pdest, (vx << 1), (vy << 1), vcol1);
			vcolm = find_most_middle ();
			put_pix_alpha_replace (pdest, (vx << 1) + 1, (vy << 1) + 1,
								   vcolm);
			put_pix_alpha_replace (pdest, (vx << 1) + 1, (vy << 1),
								   find_most_upper ());
			put_pix_alpha_replace (pdest, (vx << 1), (vy << 1) + 1,
								   find_most_left ());
		}
	}
}

/* Redimentionne l'image psrc et place le resultat */
/* a la postion (0,0) dans pdest en fusionnant les */
/* points. L'image résultat doit être plus petite  */
/* que l'image d'origine. L'algo est plus simple,  */
/* moins exact mais génère moins de flou.          */
void
image_downsize_rought (image * pdest, image * psrc, int32 width, int32 height)
{
    int32 vx, vy;
    pix vcol;
    float rx, ry, rx_next, ry_next;
    float width_scale, height_scale;
    float red, green, blue, alpha;
    float factor;
    int32 i, j;

    if ((pdest == NULL) || (psrc == NULL))
		return;

    if ((psrc->width < width) || (psrc->height < height)) {
		image_resize_bilinear (pdest, psrc, width, height);
		return;
    }

    width_scale = (float) psrc->width / (float) width;
    height_scale = (float) psrc->height / (float) height;

	for (vy = 0; vy < height; vy++) {
		for (vx = 0; vx < width; vx++) {
			rx = vx * width_scale;
			ry = vy * height_scale;

			red = green = blue = alpha = 0.0;

			rx_next = rx + width_scale;
			ry_next = ry + width_scale;
			factor = 0;

			for (j = (int32) rx; (float) j < rx_next; j++) {
				for (i = (int32) ry; (float) i < ry_next; i++) {
					factor += 1;
					vcol = get_pix (psrc, j, i);

					red += (float) COL_RED (vcol);
					green += (float) COL_GREEN (vcol);
					blue += (float) COL_BLUE (vcol);
					alpha += (float) COL_ALPHA (vcol);
				}
			}

			red /= factor;
			green /= factor;
			blue /= factor;
			alpha /= factor;

			/* on sature les valeurs */
			red = (red > 255.0) ? 255.0 : ((red < 0.0) ? 0.0 : red);
			green = (green > 255.0) ? 255.0 : ((green < 0.0) ? 0.0 : green);
			blue = (blue > 255.0) ? 255.0 : ((blue < 0.0) ? 0.0 : blue);
			alpha = (alpha > 255.0) ? 255.0 : ((alpha < 0.0) ? 0.0 : alpha);

			put_pix_alpha_replace (pdest, vx, vy,
								   COL_FULL ((uint8) red, (uint8) green,
								   (uint8) blue, (uint8) alpha));
		}
	}
}

/* dessine un point aux coordonnees (x,y) de couleur col  */
void
put_pix (image * pimage, int32 x, int32 y, pix col, AlphaType alpha_type)
{
    uint32 a, inv_a;

	if ((x >= 0) && (y >= 0) && (x < pimage->width) && (y < pimage->height)) {
		switch (alpha_type) {
		  case ALPHA_REPLACE:
			pimage->buf[(y * pimage->width) + x] = col;
			break;
		  case ALPHA_KEEP:
			pimage->buf[(y * pimage->width) + x] =
						(col & (RED_MASK | GREEN_MASK | BLUE_MASK)) |
							(pimage->buf[(y * pimage->width) + x] & ALPHA_MASK);
			break;
		  case ALPHA_USE:
			a = COL_ALPHA (col);
			inv_a = 255 - a;
			pimage->buf[(y * pimage->width) + x] = COL_FULL(((COL_RED(pimage->buf[(y * pimage->width) + x]) * inv_a) + (COL_RED (col) * a)) / 255,
									 ((COL_GREEN(pimage->buf[(y * pimage->width) + x]) * inv_a) + (COL_GREEN (col) * a)) / 255,
									 ((COL_BLUE(pimage->buf[(y * pimage->width) + x]) * inv_a) + (COL_BLUE (col) * a)) / 255,
									 COL_ALPHA(pimage->buf[(y * pimage->width) + x]));
			break;
		}
	}
}

/* dessine un point aux coordonnees (x,y) de couleur col  */
/* la transparence resultante dans pimage de celle de col */
void
put_pix_alpha_replace (image * pimage, int32 x, int32 y, pix col)
{
    if ((x >= 0) && (y >= 0) && (x < pimage->width) && (y < pimage->height))
		pimage->buf[(y * pimage->width) + x] = col;
}

/* renvoi le point aux coordonnees (x,y) */
pix
get_pix (image * pimage, int32 x, int32 y)
{
    if ((x >= 0) && (y >= 0) && (x < pimage->width) && (y < pimage->height))
		return (pimage->buf[(y * pimage->width) + x]);
    else {
		pix vpix = BLACK;
		return (vpix);
    }
}
# endif //RESIZE_JPEG
