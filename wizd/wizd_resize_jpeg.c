/****************************************************/
/* Quelques definitions utiles partout              */
/* general.h                                        */
/*                                                  */
/* Ecrit par : Daniel Lacroix (all rights reserved) */
/*                                                  */
/* This code is mostly take from Daniel.  His web   */
/*                                                  */
/* http://lab.erasme.org/resize_image/index.html    */
/*                                                  */
/****************************************************/

# ifdef RESIZE_JPEG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "wizd.h"
#include "wizd_resize_jpeg.h"

#define HAVE_BOOLEAN
#include <jpeglib.h>
#include <setjmp.h>

jmp_buf setjmp_buffer;

# define JPEG_SEND_BUFFER_SIZE	(1024 * 128)

/* input/output file format */
enum { TYPE_PPM, TYPE_PGM, TYPE_PBM, TYPE_BMP, TYPE_GIF, TYPE_TGA, TYPE_JPG, TYPE_UNK };

unsigned char	*pOutputMemory;
int				 nMaxMemory;
int				 nUsedMemory = 0;
int				 nError = 0;
static struct jpeg_destination_mgr	dest_mgr;

static void
init_destination(j_compress_ptr cinfo)
{
    cinfo->dest->next_output_byte = pOutputMemory;
    cinfo->dest->free_in_buffer = nMaxMemory;
}

static boolean
empty_output_buffer(j_compress_ptr cinfo)
{
	char *p;
	int  used;

	used = nMaxMemory;

	nMaxMemory *= 3;
	p = (char *) malloc(nMaxMemory);
	if (!p)
		return(FALSE);

	memcpy(p, pOutputMemory, used);

	debug_log_output("free ooutput memory\n");
	free(pOutputMemory);
	debug_log_output("output memory freed\n");
	pOutputMemory = p;

    cinfo->dest->free_in_buffer = nMaxMemory - used;
    cinfo->dest->next_output_byte = pOutputMemory + used;
	return(TRUE);
}

static void
term_destination(j_compress_ptr cinfo)
{
    nUsedMemory = nMaxMemory - cinfo->dest->free_in_buffer;
    //printf("nMax %d, free %d, ret %d\n", nMaxMemory, cinfo->dest->free_in_buffer, nUsedMemory);
}

static void
setupDestManager(j_compress_ptr cinfo, unsigned char *pBuf, int nBufMax)
{
    pOutputMemory = pBuf;
    nMaxMemory = nBufMax;
    dest_mgr.init_destination = &init_destination;
    dest_mgr.empty_output_buffer = &empty_output_buffer;
    dest_mgr.term_destination = &term_destination;
    cinfo->dest = &dest_mgr;
    nError = 0;
}

static void
g_JPEGFatalErrorHandler(j_common_ptr cinfo)
{
  cinfo->err->output_message(cinfo);
  longjmp(setjmp_buffer,1);
  return;
}

/*******************************************************/
/* Sauvegarde l'image pimage dans le fichier file      */
/* sous le format JPEG. Return -1 = si ERREUR, 0 sinon */
int
image_save_to_jpeg_mem(image *pimage, int jpeg_quality, char *pBuf, int nBufMax)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride;
	char *data;
	int i,x;
		 
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jerr.error_exit = g_JPEGFatalErrorHandler;  
	 
	 /* error handler to longjmp to, we want to preserve signals */
    if (setjmp(setjmp_buffer)) {
		jpeg_destroy_compress(&cinfo);
		return -1;
	}
		 
	setupDestManager(&cinfo, pBuf, nBufMax);

	cinfo.image_width = pimage->width;
	cinfo.image_height = pimage->height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, (jpeg_quality * JPEG_QUALITY) >> 8, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	row_stride = cinfo.image_width * 3;
	if ((data = (char *) malloc(row_stride)) == NULL)
		return(-1);

	i = 0;
    while(cinfo.next_scanline < cinfo.image_height) {
		for(x = 0; x < pimage->width; x++) {
			data[x+x+x]   = COL_RED(pimage->buf[i]);
			data[x+x+x+1] = COL_GREEN(pimage->buf[i]);
			data[x+x+x+2] = COL_BLUE(pimage->buf[i]);
			i++;
		}

		row_pointer[0] = data;

		/*im->rgb_data + (cinfo.next_scanline * row_stride);*/
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);

	free(data);

	return 0;
}

int
send_jpeg_file(int accept_socket, char *buf, off_t content_length)
{
	int	send_header_data_len;
	unsigned char	send_http_header_buf[2048];
	unsigned char	work_buf[1024];
	int				result_len;
	off_t		    total_read_size;
	int				target_read_size;

	strncpy(send_http_header_buf, HTTP_OK, sizeof(send_http_header_buf));

	strncat(send_http_header_buf, HTTP_CONNECTION, sizeof(send_http_header_buf) - strlen(send_http_header_buf));

	snprintf(work_buf, sizeof(work_buf), HTTP_SERVER_NAME, SERVER_NAME);
	strncat(send_http_header_buf, work_buf, sizeof(send_http_header_buf) - strlen(send_http_header_buf));

	snprintf(work_buf, sizeof(work_buf), HTTP_CONTENT_LENGTH, content_length);
	strncat(send_http_header_buf, work_buf, sizeof(send_http_header_buf) - strlen(send_http_header_buf) );

	snprintf(work_buf, sizeof(work_buf), HTTP_CONTENT_TYPE, "image/jpeg");
	strncat(send_http_header_buf, work_buf, sizeof(send_http_header_buf) - strlen(send_http_header_buf) );
	strncat(send_http_header_buf, HTTP_END, sizeof(send_http_header_buf) - strlen(send_http_header_buf) );


	send_header_data_len = strlen(send_http_header_buf);
	debug_log_output("send_header_data_len for jpeg = %d\n", send_header_data_len);
	debug_log_output("--------\n");
	debug_log_output("%s", send_http_header_buf);
	debug_log_output("--------\n");

	result_len = send(accept_socket, send_http_header_buf, send_header_data_len, 0);

	total_read_size = 0;
	while ((content_length==0) || (total_read_size < content_length)) {
		if ( (content_length - total_read_size) > JPEG_SEND_BUFFER_SIZE )
			target_read_size = JPEG_SEND_BUFFER_SIZE;
		else
			target_read_size = (size_t)(content_length - total_read_size);

		result_len = send(accept_socket, buf + total_read_size, target_read_size, 0);
		if ( result_len != target_read_size ) {
			debug_log_output("send() error.\n");
			return(total_read_size);
		}

		debug_log_output("jpeg sent %d bytes\n", result_len);

		total_read_size += target_read_size;
	}

	// printf("send %lld bytes, needed to send %lld bytes\n", total_read_size, content_length);
	return(total_read_size);
}

int
wizd_do_resize(char *file_src, int height, int width, int mode, int fd)
{
	image *vimage = NULL;
	image *vimage_resize = NULL;
	int	   jpeg_quality=100;
	int	   nMaxBuf;
	int	   len;
	char  *pNewJpeg;

	debug_log_output("got width %d, height %d, maxheight %d, maxwidth %d\n", width, height, global_param.max_jpeg_height, global_param.max_jpeg_width);

	if ((vimage = image_new_from_jpeg(file_src)) == NULL)
		return(-1);

	if (height == 0 && width == 0) {
		height = vimage->height;
		width = vimage->width;
	} else if(height <= 0)
		height = (width * vimage->height) / vimage->width;
	else if(width <= 0)
		width = (height * vimage->width) / vimage->height;

	if (global_param.max_jpeg_height && height > global_param.max_jpeg_height) {
		height = global_param.max_jpeg_height;
		width = (height * vimage->width) / vimage->height;
	}
	
	if (global_param.max_jpeg_width && width > global_param.max_jpeg_width) {
		width = global_param.max_jpeg_width;
		height = (width * vimage->height) / vimage->width;
	}

	if (width == vimage->width && height == vimage->height) {
		// no resize needed
		debug_log_output("no resize needed\n");
		image_free(vimage);
		return(-1);
	}

	debug_log_output("using height %d, width %d\n", height, width);

	/* create the destination image */  
	if ((vimage_resize = image_new(width, height)) == NULL)
		return(-1);

	nMaxBuf = (width * height * 4);
	pNewJpeg = (unsigned char *) malloc(nMaxBuf);

	switch(mode)
	{
	  case NEAREST     :
		image_resize_nearest(vimage_resize,vimage,width,height);
		break;
	  case BILINEAR    :
		image_resize_bilinear(vimage_resize,vimage,width,height);
		break;
	  case DCT         :
		image_resize_dct(vimage_resize,vimage,width,height);
		break;
	  case CATMULL     :
		image_resize_catmull_rom(vimage_resize,vimage,width,height);
		break;
	  case TV          :
		image_resize_tv(vimage_resize,vimage);
		break;
	  case TV2         :
		image_resize_tv2(vimage_resize,vimage);
		break;
	  case MOST        :
		image_resize_most(vimage_resize,vimage);
		break;
	  case DOWN        :
		image_downsize(vimage_resize,vimage,width,height);
		break;
	  case DOWN_ROUGHT :
		image_downsize_rought(vimage_resize,vimage,width,height);
		break;
	}

	debug_log_output("did image resize\n");

	if (image_save_to_jpeg_mem(vimage_resize, jpeg_quality, pNewJpeg, nMaxBuf)){
		image_free(vimage);
		image_free(vimage_resize);
		free(pNewJpeg);
		return(-1);
	}

	debug_log_output("did image save\n");

	if (nUsedMemory) {
# if 0
		http_send_ok_header(fd, nUsedMemory, "image/jpeg");
		len = send(fd, pNewJpeg, nUsedMemory, 0);
# else
		len = send_jpeg_file(fd, pNewJpeg, (off_t) nUsedMemory);
		debug_log_output("did image send\n");
# endif

# if 0
		debug_log_output("sending %d bytes\n", nUsedMemory);
		{
			FILE *fpw;
			char buf[256];

			sprintf(buf, "%s.save.jpg", file_src);
			printf("Creating file %s\n", buf);
			fpw = fopen(buf, "wb");
			fwrite(pNewJpeg, nUsedMemory, 1, fpw);
			fclose(fpw);
		}
# endif
	}

	debug_log_output("calling image_free1\n");
	image_free(vimage);

	debug_log_output("calling image_free2\n");
	image_free(vimage_resize);

	debug_log_output("calling free\n");
	free(pNewJpeg);

	debug_log_output("all done\n");


	return(0);
}

int
http_send_resized_jpeg(int fd, HTTP_RECV_INFO *http_recv_info_p)
{
	char  *p;

	// make sure this is a jpg file
	p = strrchr(http_recv_info_p->send_filename, '.');
	if (!p)
		return(0);
	p++;

	if (strcasecmp(p, "jpeg") != 0 && strcasecmp(p, "jpg") != 0) {
		// printf("skipped %s\n", http_recv_info_p->send_filename);
		return(0);
	}

	debug_log_output("name %s, height %d, width %d, mode %d\n", http_recv_info_p->send_filename, global_param.target_jpeg_height,global_param.target_jpeg_width, global_param.target_jpeg_mode);

	if (wizd_do_resize(http_recv_info_p->send_filename,
					   global_param.target_jpeg_height,
					   global_param.target_jpeg_width,
					   global_param.target_jpeg_mode,
					   fd) != 0)
		return(0);

	if (!nUsedMemory)
		return(0);

	return(1);
}

# endif //RESIZE_JPEG
