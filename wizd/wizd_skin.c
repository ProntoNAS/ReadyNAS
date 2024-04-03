#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "wizd.h"
#include "wizd_skin.h"

static unsigned char	skin_path[WIZD_FILENAME_MAX];

// prototype
unsigned char *skin_file_read(unsigned char *read_filename, unsigned long *malloc_size);

SKIN_T *skin_open(char *filename)
{
	char read_filename[WIZD_FILENAME_MAX];
	SKIN_T *skin;

	skin = malloc(sizeof(SKIN_T));
	if (skin == NULL) {
		debug_log_output("open_skin: malloc error");
		return NULL;
	}

	snprintf(read_filename, sizeof(read_filename), "%s/%s/%s", global_param.skin_root, global_param.skin_name, filename);
	skin->buffer = skin_file_read(read_filename, &skin->buffer_size);
	if (skin->buffer == NULL) {
		debug_log_output("open_skin: skin_file_read() error");
		free(skin);
		return NULL;
	}

	return skin;
}

void skin_close(SKIN_T *skin)
{
	if (skin != NULL) {
		if (skin->buffer != NULL) free(skin->buffer);
		free(skin);
	}
}

void skin_read_config(char *filename)
{
	char read_filename[WIZD_FILENAME_MAX];

	snprintf(skin_path, sizeof(skin_path), "%s/%s"
		, global_param.skin_root
		, global_param.skin_name
	);
	snprintf(read_filename, sizeof(read_filename), "%s/%s", skin_path, filename);

	skin_config_file_read(read_filename);
}

unsigned char* skin_get_string(SKIN_T *skin)
{
	return skin->buffer;
}

void skin_direct_replace_string(SKIN_T *skin, unsigned char *orig, unsigned char *str)
{
	replase_character(skin->buffer, skin->buffer_size, orig, str);
}

void skin_direct_replace_format(SKIN_T *skin, unsigned char *orig, unsigned char *fmt, ...)
{
	va_list arg;
	unsigned char work_buf[1024]; // is it enough??

	va_start(arg, fmt);
	vsnprintf(work_buf, sizeof(work_buf), fmt, arg);
	va_end(arg);

	skin_direct_replace_string(skin, orig, work_buf);
}

void skin_direct_replace_global(SKIN_T *skin, SKIN_REPLASE_GLOBAL_DATA_T *rep_p)
{
	replase_skin_grobal_data(skin->buffer, skin->buffer_size, rep_p);
}

void skin_direct_cut_enclosed_words(SKIN_T *skin, unsigned char *s, unsigned char *e)
{
	cut_enclose_words(skin->buffer, skin->buffer_size, s, e);
}

int skin_direct_send(int fd, SKIN_T *skin)
{
	return send(fd, skin->buffer, strlen(skin->buffer), 0);
}

// **************************************************************************
// スキンファイルを読み込む。
//
//  ファイルサイズにあわせて、malloc()
//  文字コード変換。
// **************************************************************************
unsigned char *skin_file_read(unsigned char *read_filename, unsigned long *malloc_size )
{
	int		fd;
	struct stat		file_stat;
	int				result;
	ssize_t			read_size;
	unsigned char 	*read_work_buf;
	unsigned char 	*read_buf;



	// ファイルサイズチェック
	result = stat(read_filename, &file_stat);
	debug_log_output("skin: head stat(%s)=%d, st_size=%lld", read_filename, result, file_stat.st_size );
	if ( result != 0 )
	{
		debug_log_output("stat(%s) error.", read_filename);
		return NULL;
	}

	// ファイルサイズの6倍の領域をmalloc(２つ)
	*malloc_size = file_stat.st_size * 6 ;

	read_work_buf = malloc( *malloc_size );
	if ( read_work_buf == NULL )
	{
		debug_log_output("maloc() error.");
		return NULL;
	}

	read_buf = malloc( *malloc_size );
	if ( read_buf == NULL )
	{
		debug_log_output("maloc() error.");
		return NULL;
	}

	memset(read_work_buf, '\0', *malloc_size);
	memset(read_buf, '\0', *malloc_size);

	// ファイル読み込み
	fd = open(read_filename, O_RDONLY );
	if ( fd < 0 )
	{
		debug_log_output("open(%s) error.",read_filename);
		free(read_buf);
		free(read_work_buf);
		return NULL;
	}

	read_size = read(fd, read_work_buf, file_stat.st_size );
	debug_log_output("skin: read() read_size=%d", read_size );

	if ( read_size != file_stat.st_size )
	{
		debug_log_output("read() error.");
		free(read_buf);
		free(read_work_buf);
		return NULL;
	}

	close( fd );

	// 読んだデータの文字コードを、MediaWiz用コードに変換
	convert_language_code(read_work_buf, read_buf, *malloc_size, CODE_AUTO, global_param.client_language_code);

	debug_log_output("skin: nkf end." );

	// ワークエリアをfree.
	free(read_work_buf);


	return read_buf;	// 正常終了
}


// ****************************************************************************************
// Create the global skin data structure
// ****************************************************************************************
SKIN_REPLASE_GLOBAL_DATA_T *skin_create_global_data(HTTP_RECV_INFO *http_recv_info_p, int file_num)
{
	unsigned char	work_data[WIZD_FILENAME_MAX];
	unsigned char	work_data2[WIZD_FILENAME_MAX];
	unsigned char	work_filename[WIZD_FILENAME_MAX];

	struct	stat	dir_stat;

	char thumb = global_param.flag_default_thumb;

	SKIN_REPLASE_GLOBAL_DATA_T *skin_rep_data_global_p=NULL;

	skin_rep_data_global_p 	= malloc( sizeof(SKIN_REPLASE_GLOBAL_DATA_T) );
	if ( skin_rep_data_global_p == NULL )
	{
		debug_log_output("malloc() error.");
		return NULL;
	}
	memset(skin_rep_data_global_p, '\0', sizeof(SKIN_REPLASE_GLOBAL_DATA_T));

	strcpy(skin_rep_data_global_p->request_uri, http_recv_info_p->request_uri);

	// ---------------------------------
	// Start of information formation for global indication
	// ---------------------------------

	if ( global_param.flag_allow_delete && (strcasecmp(http_recv_info_p->action, "delete") == 0) ) {
		debug_log_output("action=delete: Enabling delete mode output");
		skin_rep_data_global_p->delete_mode = 1;
	}

	if (strcasecmp(http_recv_info_p->option, "thumb") == 0) {
		debug_log_output("option=thumb: Enabling thumbnail mode");
		thumb = 1;
	} else if(strcasecmp(http_recv_info_p->option, "details") == 0) {
		debug_log_output("option=details: Enabling details listing mode");
		thumb = 0;
	}
	if(thumb) {	
		debug_log_output("thumb_row_max = %d", global_param.thumb_row_max);
		debug_log_output("thumb_column_max = %d", global_param.thumb_column_max);
		skin_rep_data_global_p->columns = global_param.thumb_column_max;
		skin_rep_data_global_p->items_per_page = global_param.thumb_row_max * global_param.thumb_column_max;
		skin_rep_data_global_p->filename_length_max = global_param.thumb_filename_length_max;
	} else {
		skin_rep_data_global_p->items_per_page = global_param.page_line_max;
		skin_rep_data_global_p->columns = 0;
		skin_rep_data_global_p->filename_length_max = global_param.menu_filename_length_max;
		debug_log_output("page_line_max = %d", global_param.page_line_max);
	}
	if(skin_rep_data_global_p->items_per_page < 1)
		skin_rep_data_global_p->items_per_page = 1;

	// Number of files
	debug_log_output("file_num = %d", file_num);
	debug_log_output("items_per_page = %d", skin_rep_data_global_p->items_per_page);

	// 最大ページ数計算
	if ( file_num == 0 )
	{
		skin_rep_data_global_p->max_page = 1;
	}
	else
	{
		skin_rep_data_global_p->max_page = ((file_num-1) / skin_rep_data_global_p->items_per_page) + 1;
	}
	debug_log_output("max_page = %d", skin_rep_data_global_p->max_page);

	// Current page calculation
	if ( (http_recv_info_p->page <= 1 ) || (skin_rep_data_global_p->max_page < http_recv_info_p->page ) )
		skin_rep_data_global_p->now_page = 1;
	else
		skin_rep_data_global_p->now_page = http_recv_info_p->page;

	debug_log_output("now_page = %d", skin_rep_data_global_p->now_page);

	// calculation of line to be shown on current page
	if ( skin_rep_data_global_p->max_page == skin_rep_data_global_p->now_page ) // 最後のページ
		skin_rep_data_global_p->now_page_line = file_num - (skin_rep_data_global_p->items_per_page * (skin_rep_data_global_p->max_page-1));
	else	// 最後以外なら、表示最大数。
		skin_rep_data_global_p->now_page_line = skin_rep_data_global_p->items_per_page;
	debug_log_output("now_page_line = %d", skin_rep_data_global_p->now_page_line);


	// Start file number calculation
	skin_rep_data_global_p->start_file_num = ((skin_rep_data_global_p->now_page - 1) * skin_rep_data_global_p->items_per_page);
	debug_log_output("start_file_num = %d", skin_rep_data_global_p->start_file_num);

	if ( skin_rep_data_global_p->max_page == skin_rep_data_global_p->now_page ) // 最後のページ
		skin_rep_data_global_p->end_file_num = file_num;
	else // not last page
		skin_rep_data_global_p->end_file_num = (skin_rep_data_global_p->start_file_num + skin_rep_data_global_p->items_per_page);
	debug_log_output("start_file_num = %d", skin_rep_data_global_p->start_file_num);

	// Prev folder calculation
	skin_rep_data_global_p->prev_page =  1 ;
	if ( skin_rep_data_global_p->now_page > 1 )
		skin_rep_data_global_p->prev_page = skin_rep_data_global_p->now_page - 1;

	// Next page/folder calculation
	skin_rep_data_global_p->next_page = skin_rep_data_global_p->max_page ;
	if ( skin_rep_data_global_p->max_page > skin_rep_data_global_p->now_page )
		skin_rep_data_global_p->next_page = skin_rep_data_global_p->now_page + 1;

	debug_log_output("prev_page=%d  next_page=%d", skin_rep_data_global_p->prev_page ,skin_rep_data_global_p->next_page);


	// Formation for present path name indication (recv_uri - current_path_name [ client code ])
	convert_language_code(	http_recv_info_p->recv_uri,
							skin_rep_data_global_p->current_path_name,
							sizeof(skin_rep_data_global_p->current_path_name),
							global_param.server_language_code | CODE_HEX,
							global_param.client_language_code );
	debug_log_output("current_path = '%s'", skin_rep_data_global_p->current_path_name );

	// -----------------------------------------------
	// Forming the one for upper directory
	// -----------------------------------------------
	work_data[0] = 0;
	work_data2[0] = 0;
	if (get_parent_path(work_data, http_recv_info_p->recv_uri, sizeof(work_data)) == NULL) {
		debug_log_output("FATAL ERROR! too long recv_uri.");
		return NULL;
	}
	debug_log_output("parent_directory='%s'", work_data);

	// Directory path with respect to one URI encoding.
	uri_encode(skin_rep_data_global_p->parent_directory_link, sizeof(skin_rep_data_global_p->parent_directory_link), work_data, strlen(work_data));
	// '?'add
	strncat(skin_rep_data_global_p->parent_directory_link, "?", sizeof(skin_rep_data_global_p->parent_directory_link) - strlen(skin_rep_data_global_p->parent_directory_link));
	// if sort
	if ( strlen(http_recv_info_p->sort) > 0 ) {
		snprintf(work_data, sizeof(work_data), "sort=%s&", http_recv_info_p->sort);
		strncat(skin_rep_data_global_p->parent_directory_link, work_data, sizeof(skin_rep_data_global_p->parent_directory_link) - strlen(skin_rep_data_global_p->parent_directory_link));
	}
	if ( strlen(http_recv_info_p->option) > 0 ) {
		snprintf(work_data, sizeof(work_data), "option=%s&", http_recv_info_p->option);
		strncat(skin_rep_data_global_p->parent_directory_link, work_data, sizeof(skin_rep_data_global_p->parent_directory_link) - strlen(skin_rep_data_global_p->parent_directory_link));
	}
	if ( strlen(http_recv_info_p->dvdopt) > 0 ) {
		snprintf(work_data, sizeof(work_data), "dvdopt=%s&", http_recv_info_p->dvdopt);
		strncat(skin_rep_data_global_p->parent_directory_link, work_data, sizeof(skin_rep_data_global_p->parent_directory_link) - strlen(skin_rep_data_global_p->parent_directory_link));
	}
	if ( strlen(http_recv_info_p->search) > 0 ) {
		snprintf(work_data, sizeof(work_data), "search%s=%s&", http_recv_info_p->search_str, http_recv_info_p->search);
		strncat(skin_rep_data_global_p->parent_directory_link, work_data, sizeof(skin_rep_data_global_p->parent_directory_link) - strlen(skin_rep_data_global_p->parent_directory_link));
	}
	debug_log_output("parent_directory_link='%s'", skin_rep_data_global_p->parent_directory_link);

	// For upper directory name indication
	convert_language_code(	work_data,
							skin_rep_data_global_p->parent_directory_name,
							sizeof(skin_rep_data_global_p->parent_directory_name),
							global_param.server_language_code | CODE_HEX,
							global_param.client_language_code );
	debug_log_output("parent_directory_name='%s'", skin_rep_data_global_p->parent_directory_link);


	// -----------------------------------------------
	// Forming for present directory next
	// -----------------------------------------------

	// Forming present directory name. First it converts to EUC.
	switch (http_recv_info_p->default_file_type) {
	  case TYPE_MOVIE:
		strcpy(skin_rep_data_global_p->default_dir_type, "movie");
		break;
	  case TYPE_MUSIC:
		strcpy(skin_rep_data_global_p->default_dir_type, "music");
		break;
	  case TYPE_JPEG:
		strcpy(skin_rep_data_global_p->default_dir_type, "photo");
		break;
	  default:
		strcpy(skin_rep_data_global_p->default_dir_type, "all");
		break;
	}
	convert_language_code(	http_recv_info_p->recv_uri,
							work_data,
							sizeof(work_data),
							global_param.server_language_code | CODE_HEX,
							CODE_EUC );

	// delete last dir char end with “/”
	cut_character_at_linetail(work_data, '/');
	// delete before  start with /
	cut_before_last_character(work_data, '/');

	// Adding ‘/’
	strncat(work_data, "/", sizeof(work_data) - strlen(work_data) );

	// CUT Exec
	euc_string_cut_n_length(work_data, skin_rep_data_global_p->filename_length_max);

	// Clean up name (remove underscores, change all upper to Title Case
	// clean_buffer_text(work_data);

	// Formation for present directory name indication (character code conversion)
	convert_language_code(	work_data,
							skin_rep_data_global_p->current_directory_name,
							sizeof(skin_rep_data_global_p->current_directory_name),
							CODE_EUC,
							global_param.client_language_code );

	debug_log_output("current_dir = '%s'", skin_rep_data_global_p->current_directory_name );


	// The formation for present path name Link (URI encoding from recv_uri)
	uri_encode(skin_rep_data_global_p->current_directory_link_no_param
		, sizeof(skin_rep_data_global_p->current_directory_link_no_param)
		, http_recv_info_p->recv_uri
		, strlen(http_recv_info_p->recv_uri)
	);
	strncpy(skin_rep_data_global_p->current_directory_link
		, skin_rep_data_global_p->current_directory_link_no_param
		, sizeof(skin_rep_data_global_p->current_directory_link)
	);

	strncpy(skin_rep_data_global_p->current_directory_absolute, "http://", sizeof(skin_rep_data_global_p->current_directory_absolute));
	strncat(skin_rep_data_global_p->current_directory_absolute, http_recv_info_p->recv_host, sizeof(skin_rep_data_global_p->current_directory_absolute) - strlen(skin_rep_data_global_p->current_directory_absolute));
	strncpy(skin_rep_data_global_p->recv_host, http_recv_info_p->recv_host, sizeof(skin_rep_data_global_p->recv_host));


	// ?を追加
	strncat(skin_rep_data_global_p->current_directory_link, "?", sizeof(skin_rep_data_global_p->current_directory_link) - strlen(skin_rep_data_global_p->current_directory_link)); // '?'を追加
	strncpy(skin_rep_data_global_p->current_directory_link_no_sort, skin_rep_data_global_p->current_directory_link, sizeof(skin_rep_data_global_p->current_directory_link_no_sort));
	strncpy(skin_rep_data_global_p->current_directory_link_no_option, skin_rep_data_global_p->current_directory_link, sizeof(skin_rep_data_global_p->current_directory_link_no_option));
	strncpy(skin_rep_data_global_p->current_directory_link_no_dvdopt, skin_rep_data_global_p->current_directory_link, sizeof(skin_rep_data_global_p->current_directory_link_no_dvdopt));

	// When using 'showchapters', we need to retain this setting when changing pages
	if (strncmp(http_recv_info_p->action, "showchapters", 12) == 0 ) {
		snprintf(work_data, sizeof(work_data), "action=%s&", http_recv_info_p->action);
		strncat(skin_rep_data_global_p->current_directory_link, work_data, sizeof(skin_rep_data_global_p->current_directory_link) - strlen(skin_rep_data_global_p->current_directory_link));
	}
	// sort=が指示されていた場合、それを引き継ぐ。
	if ( strlen(http_recv_info_p->search) > 0 )
	{
		snprintf(work_data, sizeof(work_data), "search%s=%s&", http_recv_info_p->search_str, http_recv_info_p->search);
		strncat(skin_rep_data_global_p->current_directory_link, work_data, sizeof(skin_rep_data_global_p->current_directory_link) - strlen(skin_rep_data_global_p->current_directory_link));
		strncat(skin_rep_data_global_p->current_directory_link_no_option, work_data, sizeof(skin_rep_data_global_p->current_directory_link_no_option) - strlen(skin_rep_data_global_p->current_directory_link_no_option));
		strncat(skin_rep_data_global_p->current_directory_link_no_dvdopt, work_data, sizeof(skin_rep_data_global_p->current_directory_link_no_dvdopt) - strlen(skin_rep_data_global_p->current_directory_link_no_dvdopt));
		if (http_recv_info_p->lsearch[0] != '\0') {
			sprintf(work_data, "lsearch=%s&", http_recv_info_p->lsearch);
			strcat(skin_rep_data_global_p->current_directory_link, work_data);
		}
	}
	if ( strlen(http_recv_info_p->sort) > 0 )
	{
		snprintf(work_data, sizeof(work_data), "sort=%s&", http_recv_info_p->sort);
		strncat(skin_rep_data_global_p->current_directory_link, work_data, sizeof(skin_rep_data_global_p->current_directory_link) - strlen(skin_rep_data_global_p->current_directory_link));
		strncat(skin_rep_data_global_p->current_directory_link_no_option, work_data, sizeof(skin_rep_data_global_p->current_directory_link_no_option) - strlen(skin_rep_data_global_p->current_directory_link_no_option));
		strncat(skin_rep_data_global_p->current_directory_link_no_dvdopt, work_data, sizeof(skin_rep_data_global_p->current_directory_link_no_dvdopt) - strlen(skin_rep_data_global_p->current_directory_link_no_dvdopt));
	}
	if ( strlen(http_recv_info_p->option) > 0 )
	{
		snprintf(work_data, sizeof(work_data), "option=%s&", http_recv_info_p->option);
		strncat(skin_rep_data_global_p->current_directory_link, work_data, sizeof(skin_rep_data_global_p->current_directory_link) - strlen(skin_rep_data_global_p->current_directory_link));
		strncat(skin_rep_data_global_p->current_directory_link_no_sort, work_data, sizeof(skin_rep_data_global_p->current_directory_link_no_sort) - strlen(skin_rep_data_global_p->current_directory_link_no_sort));
		strncat(skin_rep_data_global_p->current_directory_link_no_dvdopt, work_data, sizeof(skin_rep_data_global_p->current_directory_link_no_dvdopt) - strlen(skin_rep_data_global_p->current_directory_link_no_dvdopt));
	}
	if ( strlen(http_recv_info_p->dvdopt) > 0 )
	{
		snprintf(work_data, sizeof(work_data), "dvdopt=%s&", http_recv_info_p->dvdopt);
		strncat(skin_rep_data_global_p->current_directory_link, work_data, sizeof(skin_rep_data_global_p->current_directory_link) - strlen(skin_rep_data_global_p->current_directory_link));
		strncat(skin_rep_data_global_p->current_directory_link_no_sort, work_data, sizeof(skin_rep_data_global_p->current_directory_link_no_sort) - strlen(skin_rep_data_global_p->current_directory_link_no_sort));
		strncat(skin_rep_data_global_p->current_directory_link_no_option, work_data, sizeof(skin_rep_data_global_p->current_directory_link_no_option) - strlen(skin_rep_data_global_p->current_directory_link_no_option));
	}
	if ( http_recv_info_p->title > 0) {
		snprintf(work_data, sizeof(work_data), "title=%d&", http_recv_info_p->title);
		strncat(skin_rep_data_global_p->current_directory_link, work_data, sizeof(skin_rep_data_global_p->current_directory_link) - strlen(skin_rep_data_global_p->current_directory_link));
	}
	debug_log_output("current_directory_link='%s'", skin_rep_data_global_p->current_directory_link);
	debug_log_output("current_directory_link_no_sort='%s'", skin_rep_data_global_p->current_directory_link_no_sort);
	debug_log_output("current_directory_link_no_option='%s'", skin_rep_data_global_p->current_directory_link_no_option);
	debug_log_output("current_directory_link_no_dvdopt='%s'", skin_rep_data_global_p->current_directory_link_no_dvdopt);
	strncat(skin_rep_data_global_p->current_directory_absolute, skin_rep_data_global_p->current_directory_link, sizeof(skin_rep_data_global_p->current_directory_absolute) - strlen(skin_rep_data_global_p->current_directory_absolute));

	debug_log_output("current_directory_absolute='%s'", skin_rep_data_global_p->current_directory_absolute);

	// ------------------------------------ processing here for directory indication

	// For directory existence file several indications
	snprintf(skin_rep_data_global_p->file_num_str, sizeof(skin_rep_data_global_p->file_num_str), "%d", file_num );

	// 	For present page indication
	snprintf(skin_rep_data_global_p->now_page_str, sizeof(skin_rep_data_global_p->now_page_str), "%d", skin_rep_data_global_p->now_page );

	// For full page several indications
	snprintf(skin_rep_data_global_p->max_page_str, sizeof(skin_rep_data_global_p->max_page_str), "%d", skin_rep_data_global_p->max_page );

	//for next page indication
	snprintf(skin_rep_data_global_p->next_page_str, sizeof(skin_rep_data_global_p->next_page_str), "%d", skin_rep_data_global_p->next_page );

	// 前のページ 表示用
	snprintf(skin_rep_data_global_p->prev_page_str, sizeof(skin_rep_data_global_p->prev_page_str), "%d", skin_rep_data_global_p->prev_page );

	// 開始ファイル番号表示用
	if ( file_num == 0 )
		snprintf(skin_rep_data_global_p->start_file_num_str, sizeof(skin_rep_data_global_p->start_file_num_str), "%d", skin_rep_data_global_p->start_file_num );
	else
		snprintf(skin_rep_data_global_p->start_file_num_str, sizeof(skin_rep_data_global_p->start_file_num_str), "%d", skin_rep_data_global_p->start_file_num +1 );

	// For end file number indication
	snprintf(skin_rep_data_global_p->end_file_num_str, sizeof(skin_rep_data_global_p->end_file_num_str), "%d", skin_rep_data_global_p->end_file_num  );

	skin_rep_data_global_p->stream_dirs = 0;	// Subdirectories
	skin_rep_data_global_p->stream_files = 0;	// Playable video files
	skin_rep_data_global_p->music_files = 0;	// Playable music files
	skin_rep_data_global_p->photo_files = 0;	// Playable photo files

	// Whether or not PC it adds to the information of skin substitution
	skin_rep_data_global_p->flag_pc = http_recv_info_p->flag_pc;
	skin_rep_data_global_p->flag_hd = http_recv_info_p->flag_hd;

	// Set up for the default playlists
	strncpy(work_filename, http_recv_info_p->send_filename, sizeof(work_filename));
	strncat(work_filename, DEFAULT_PHOTOLIST, sizeof(work_filename)-strlen(work_filename));
	if( (0 == stat(work_filename, &dir_stat)) && S_ISREG(dir_stat.st_mode) ) {
		snprintf(skin_rep_data_global_p->default_photolist, sizeof(skin_rep_data_global_p->default_photolist), "pod=\"2,1,%s%s?type=slideshow\"",
			skin_rep_data_global_p->current_directory_absolute, DEFAULT_PHOTOLIST);
	} else {
		snprintf(work_filename, sizeof(work_filename), "%s/%s/%s", global_param.skin_root, global_param.skin_name, DEFAULT_PHOTOLIST);
		duplex_character_to_unique(work_filename, '/');
		if( (0 == stat(work_filename, &dir_stat)) && S_ISREG(dir_stat.st_mode) ) {
			snprintf(skin_rep_data_global_p->default_photolist, sizeof(skin_rep_data_global_p->default_photolist), 
				"pod=\"2,1,http://%s/%s?type=slideshow&sort=shuffle\"", http_recv_info_p->recv_host, DEFAULT_PHOTOLIST);
		} else {
			// No default photo list
			strncpy(skin_rep_data_global_p->default_photolist, "vod=\"playlist\"", sizeof(skin_rep_data_global_p->default_photolist));
		}
	}

	debug_log_output("Default photolist = '%s'", skin_rep_data_global_p->default_photolist);

	strncpy(work_filename, http_recv_info_p->send_filename, sizeof(work_filename));
	strncat(work_filename, DEFAULT_MUSICLIST, sizeof(work_filename)-strlen(work_filename));
	if( (0 == stat(work_filename, &dir_stat)) && S_ISREG(dir_stat.st_mode) ) {
		strncpy(skin_rep_data_global_p->default_musiclist, DEFAULT_MUSICLIST, sizeof(skin_rep_data_global_p->default_musiclist));
	} else {
		snprintf(work_filename, sizeof(work_filename), "%s/%s/%s", global_param.skin_root, global_param.skin_name, DEFAULT_MUSICLIST);
		duplex_character_to_unique(work_filename, '/');
		if( (0 == stat(work_filename, &dir_stat)) && S_ISREG(dir_stat.st_mode) ) {
			strncpy(skin_rep_data_global_p->default_musiclist, DEFAULT_MUSICLIST, sizeof(skin_rep_data_global_p->default_musiclist));
			// Always shuffle the default music list
			strncat(skin_rep_data_global_p->default_musiclist, "?sort=shuffle", sizeof(skin_rep_data_global_p->default_musiclist) - strlen(skin_rep_data_global_p->default_musiclist));
		} else {
			// No default musiclist
			strncpy(skin_rep_data_global_p->default_musiclist, "mute", sizeof(skin_rep_data_global_p->default_musiclist));
		}
	}

	debug_log_output("Default musiclist = '%s'", skin_rep_data_global_p->default_musiclist);

	// BODY tag onloadset= "$focus"
	if (http_recv_info_p->focus[0]) {
		// For safety uri_encode it does first.
		// (to prevent, $focus = "start\"><a href=\"...."; hack. ;p)
		uri_encode(work_data, sizeof(work_data)
			, http_recv_info_p->focus, strlen(http_recv_info_p->focus));
		snprintf(skin_rep_data_global_p->focus
			, sizeof(skin_rep_data_global_p->focus)
			, " onloadset=\"%s\"", work_data);

	} else {
		skin_rep_data_global_p->focus[0] = '\0'; /* nothing :) */
	}

	return skin_rep_data_global_p;
}



// **************************************************************************
// 全体用データ まとめて置換
// **************************************************************************
void replase_skin_grobal_data(unsigned char *menu_work_p, int menu_work_buf_size, SKIN_REPLASE_GLOBAL_DATA_T *skin_rep_data_global_p)
{
	unsigned char	current_date[32];	// 現在の日付表示用
	unsigned char	current_time[32];	// 現在の時刻表示用
	char			workbuf[256];
	int				i;

	// ===============================
	// = 削除キーワードを削除する
	// ===============================
#define REPLACE(a,b) replase_character(menu_work_p, menu_work_buf_size,\
			SKIN_KEYWORD_##a, (b))
#define REPLACE_G(a,b) replase_character(menu_work_p, menu_work_buf_size,\
			SKIN_KEYWORD_##a, skin_rep_data_global_p->b)

#define DELETE(a) cut_enclose_words(menu_work_p, menu_work_buf_size,\
			SKIN_KEYWORD_DEL_##a, SKIN_KEYWORD_DEL_##a##_E)

	// ルートパスの場合。
	if ( strcmp(skin_rep_data_global_p->current_path_name, "/" ) == 0 ) {
		DELETE(IS_ROOTDIR);
	}

	// １ページの場合
	if ( strcmp(skin_rep_data_global_p->now_page_str, "1" ) == 0 ) {
		DELETE(IS_NO_PAGE_PREV);
		DELETE(IS_NO_PAGE_PREV2);
	} else {
		DELETE(IS_PAGE_PREV);
	}

	// 最後のページの場合
	if ( strcmp(skin_rep_data_global_p->now_page_str, skin_rep_data_global_p->max_page_str) == 0 ) {
		DELETE(IS_NO_PAGE_NEXT);
		DELETE(IS_NO_PAGE_NEXT2);
	} else {
		DELETE(IS_PAGE_NEXT);
	}

	// 再生可能ファイル数が０の場合
	if ( skin_rep_data_global_p->stream_files == 0 ) {
		DELETE(IS_NO_STREAM_FILES);
		DELETE(IS_NO_STREAM_FILES2);
	} else {
		DELETE(IS_STREAM_FILES);
	}
	if ( skin_rep_data_global_p->music_files == 0 ) {
		DELETE(IS_NO_MUSIC_FILES);
		DELETE(IS_NO_MUSIC_FILES2);
	} else {
		DELETE(IS_MUSIC_FILES);
	}
	if ( skin_rep_data_global_p->photo_files == 0 ) {
		DELETE(IS_NO_PHOTO_FILES);
		DELETE(IS_NO_PHOTO_FILES2);
	} else {
		DELETE(IS_PHOTO_FILES);
	}

	// Remove any delete references when this mode is disabled
	if ( global_param.flag_allow_delete == 0 ) {
		DELETE(CAN_DELETE);
	}

	if ( skin_rep_data_global_p->flag_hd == 1 ) {
		DELETE(IF_CLIENT_IS_NOT_HD);
	} else {
		DELETE(IF_CLIENT_IS_HD);
	}

	if ( skin_rep_data_global_p->flag_pc == 1 ) {
		DELETE(IF_CLIENT_IS_NOT_PC);
	} else {
		DELETE(IF_CLIENT_IS_PC);
	}

	// ?focus が指定されているときとそうじゃないとき
	if ( skin_rep_data_global_p->focus[0] == '\0' ) {
		// focus が指定されていないときは 削除
		DELETE(IF_FOCUS_IS_NOT_SPECIFIED);
	} else {
		// focus が指定されているときは 削除
		DELETE(IF_FOCUS_IS_SPECIFIED);
	}

	// favorites
	for (i = 1; i < 11; i++) {
		int test;

		test = global_param.favorites[i - 1][0] == '\0';

		switch (i) {
		  case 1:
			if (test)
				DELETE(IS_NO_FAVORITES1);
			else
				REPLACE(FAVORITES1, global_param.favorites[i - 1]);
			break;
		  case 2:
			if (test)
				DELETE(IS_NO_FAVORITES2);
			else
				REPLACE(FAVORITES2, global_param.favorites[i - 1]);
			break;
		  case 3:
			if (test)
				DELETE(IS_NO_FAVORITES3);
			else
				REPLACE(FAVORITES3, global_param.favorites[i - 1]);
			break;
		  case 4:
			if (test)
				DELETE(IS_NO_FAVORITES4);
			else
				REPLACE(FAVORITES4, global_param.favorites[i - 1]);
			break;
		  case 5:
			if (test)
				DELETE(IS_NO_FAVORITES5);
			else
				REPLACE(FAVORITES5, global_param.favorites[i - 1]);
			break;
		  case 6:
			if (test)
				DELETE(IS_NO_FAVORITES6);
			else
				REPLACE(FAVORITES6, global_param.favorites[i - 1]);
			break;
		  case 7:
			if (test)
				DELETE(IS_NO_FAVORITES7);
			else
				REPLACE(FAVORITES7, global_param.favorites[i - 1]);
			break;
		  case 8:
			if (test)
				DELETE(IS_NO_FAVORITES8);
			else
				REPLACE(FAVORITES8, global_param.favorites[i - 1]);
			break;
		  case 9:
			if (test)
				DELETE(IS_NO_FAVORITES9);
			else
				REPLACE(FAVORITES9, global_param.favorites[i - 1]);
			break;
		  case 10:
			if (test)
				DELETE(IS_NO_FAVORITES10);
			else
				REPLACE(FAVORITES10, global_param.favorites[i - 1]);
			break;
		}
	}

#undef DELETE
	// =============
	// = 置換実行
	// =============

	conv_time_to_date_string(current_date, time(NULL));
	conv_time_to_string(current_time, time(NULL));

	// SERVER_NAME
	REPLACE(SERVER_NAME, SERVER_NAME);
	REPLACE(CURRENT_DATE, current_date);
	REPLACE(CURRENT_TIME, current_time);

	if (global_param.flag_show_audio_info != 0)
		REPLACE(DVD_OPTIONS, "showaudio");
	else if (global_param.flag_split_vob_chapters == 1)
		REPLACE(DVD_OPTIONS, "splitchapters");
	else if (global_param.flag_split_vob_chapters == 2)
		REPLACE(DVD_OPTIONS, "notitlesplit");
	else
		REPLACE(DVD_OPTIONS, "none");

	if (global_param.flag_default_search_by_alias)
		REPLACE(ALIAS_SEARCH, "checked");
	else
		REPLACE(ALIAS_SEARCH, "");

	if (global_param.search[0] != '\0')
		REPLACE(SEARCH_STR, global_param.search);
	else
		REPLACE(SEARCH_STR, "");

	if (global_param.default_search_alias[0] != '\0') {
		sprintf(workbuf, "&lsearch=/%s/", global_param.default_search_alias);
		REPLACE(DEFAULT_SEARCH_ALIAS, workbuf);
	} else
		REPLACE(DEFAULT_SEARCH_ALIAS, "");

	REPLACE(CLIENT_CHARSET, global_param.client_language_code == CODE_SJIS ? "Shift_JIS" : "iso-8859-1");

	REPLACE_G(SERVER_ADDRESS, recv_host);
	REPLACE_G(CURRENT_REQUEST_URI, request_uri);
	REPLACE_G(CURRENT_PATH, current_path_name);
	REPLACE_G(CURRENT_DIR_NAME, current_directory_name);
	REPLACE_G(CURRENT_PATH_LINK, current_directory_link);
	REPLACE_G(CURRENT_PATH_FULL_LINK, current_directory_absolute);
	REPLACE_G(CURRENT_PATH_LINK_NO_PARAM, current_directory_link_no_param);
	REPLACE_G(CURRENT_PATH_LINK_NO_SORT, current_directory_link_no_sort);
	REPLACE_G(CURRENT_PATH_LINK_NO_DVDOPT, current_directory_link_no_dvdopt);
	REPLACE_G(CURRENT_PATH_LINK_NO_OPTION, current_directory_link_no_option);
	REPLACE_G(PARLENT_DIR_LINK, parent_directory_link );
	REPLACE_G(CURRENT_PAGE, now_page_str);
	REPLACE_G(MAX_PAGE, max_page_str);
	REPLACE_G(NEXT_PAGE, next_page_str);
	REPLACE_G(PREV_PAGE, prev_page_str);
	REPLACE_G(FILE_NUM, file_num_str);
	REPLACE_G(START_FILE_NUM, start_file_num_str);
	REPLACE_G(END_FILE_NUM, end_file_num_str);
	REPLACE_G(ONLOADSET_FOCUS, focus);
	REPLACE_G(SECRET_DIR_LINK, secret_dir_link_html);
	REPLACE_G(DEFAULT_TYPE, default_dir_type);

	REPLACE_G(DEFAULT_PHOTOLIST, default_photolist);
	REPLACE_G(DEFAULT_MUSICLIST, default_musiclist);

	REPLACE_G(BACKGROUND, background_image);

#undef REPLACE
#undef REPLACE_G

	return;
}

// **************************************************************************
// ライン用データ まとめて置換
// **************************************************************************
void replase_skin_line_data(unsigned char *menu_work_p, int menu_work_buf_size, SKIN_REPLASE_LINE_DATA_T *skin_rep_data_line_p)
{
	unsigned char	row_string[16];	// 行番号

	// ===============================
	// = 削除キーワードを削除する
	// ===============================
#define REPLACE(a,b) replase_character(menu_work_p, menu_work_buf_size,\
			SKIN_KEYWORD_LINE_##a, (b))
#define REPLACE_L(a,b) replase_character(menu_work_p, menu_work_buf_size,\
			SKIN_KEYWORD_LINE_##a, skin_rep_data_line_p->b)

#define DELETE(a) cut_enclose_words(menu_work_p, menu_work_buf_size,\
			SKIN_KEYWORD_DEL_##a, SKIN_KEYWORD_DEL_##a##_E)

	if (!global_param.flag_read_mp3_tag)
		DELETE(IS_NO_MP3_TAGS);		// MP3タグ無い場合
	else if (strcasecmp(skin_rep_data_line_p->file_extension, "mp3") == 0) {
		DELETE(IS_HAVE_MP3_TAGS);	// MP3タグ存在してる場合
	} else {
		DELETE(IS_NO_MP3_TAGS);		// MP3タグ無い場合
	}

	if (!global_param.flag_read_avi_tag)
		DELETE(IS_NOT_HAVE_AVI_TAGS);
	else if (strcasecmp(skin_rep_data_line_p->file_extension, "avi") == 0) {
		DELETE(IS_HAVE_AVI_TAGS);
	} else
		DELETE(IS_NOT_HAVE_AVI_TAGS);

	if ( skin_rep_data_line_p->row_num % 2 == 0 ) {
		DELETE(IF_LINE_IS_EVEN);	// 偶数行
	} else {
		DELETE(IF_LINE_IS_ODD);		// 奇数行
	}

	if ( skin_rep_data_line_p->is_current_page == 0 ) {
		DELETE(IS_CURRENT_PAGE);	// 偶数行
	} else {
		DELETE(IS_NOT_CURRENT_PAGE);	// 偶数行
	}

	if (skin_rep_data_line_p->html_link[0] != 0) {
		REPLACE_L(HTML_LINK, html_link);
		DELETE(IF_NO_HTML_LINK);
		// printf("html file is %s\n", skin_rep_data_line_p->html_link);
	} else {
		DELETE(IF_HAVE_HTML_LINK);
	}

	if (skin_rep_data_line_p->file_image[0] != 0) {
		DELETE(IF_NO_FILE_IMAGE);
	} else {
		DELETE(IF_HAVE_FILE_IMAGE);
	}

#undef DELETE

	// =============
	//  置換実行
	// =============

	snprintf(row_string, sizeof(row_string), "%d"
		, skin_rep_data_line_p->row_num );

	REPLACE(COLUMN_NUM, row_string);	// oops. backward compatibility :(
	REPLACE(ROW_NUM, row_string);		// 行番号

	REPLACE_L(FULL_PATHNAME, full_pathname);

	REPLACE_L(FILE_NAME, file_name);
	REPLACE_L(FILE_NAME_NO_EXT, file_name_no_ext);
	REPLACE_L(FILE_EXT, file_extension);
	REPLACE_L(FILE_IMAGE, file_image);

	REPLACE_L(INFO_LINK, info_link);

	REPLACE_L(FILE_LINK, file_uri_link);
	REPLACE_L(CHAPTER_LINK, chapter_link);
	REPLACE_L(CHAPTER_STR, chapter_str);
	REPLACE_L(TIMESTAMP, file_timestamp);
	REPLACE_L(FILE_DATE, file_timestamp_date);
	REPLACE_L(FILE_TIME, file_timestamp_time);
	REPLACE_L(FILE_DURATION, file_duration);
	REPLACE_L(TVID, tvid_string);

	REPLACE_L(FILE_VOD, vod_string);
	REPLACE_L(FILE_SIZE, file_size_string);

	REPLACE_L(SVI_INFO, svi_info_data);
	REPLACE_L(SVI_REC_TIME, svi_rec_time_data);

	REPLACE_L(IMAGE_WIDTH, image_width);
	REPLACE_L(IMAGE_HEIGHT, image_height);

	REPLACE_L(MP3TAG_TITLE, mp3_id3v1_title);
	REPLACE_L(MP3TAG_ALBUM, mp3_id3v1_album);
	REPLACE_L(MP3TAG_ARTIST, mp3_id3v1_artist);
	REPLACE_L(MP3TAG_YEAR, mp3_id3v1_year);
	REPLACE_L(MP3TAG_COMMENT,mp3_id3v1_comment);
	REPLACE_L(MP3TAG_TITLE_INFO, mp3_id3v1_title_info);
	REPLACE_L(MP3TAG_TITLE_INFO_LIMITED, mp3_id3v1_title_info_limited);

	REPLACE_L(MP3TAG_GENRE, mp3_id3v1_genre); 
	REPLACE_L(MP3TAG_BITRATE,  mp3_id3v1_bitrate);
	REPLACE_L(MP3TAG_STEREO,  mp3_id3v1_stereo);
	REPLACE_L(MP3TAG_FREQ,  mp3_id3v1_frequency);
	REPLACE_L(MP3TAG_TRACK,  mp3_id3v1_track);

	REPLACE_L(AVI_FPS, avi_fps);
	REPLACE_L(AVI_DURATION, avi_duration);
	REPLACE_L(AVI_VCODEC, avi_vcodec);
	REPLACE_L(AVI_ACODEC, avi_acodec);
	REPLACE_L(AVI_HVCODEC, avi_hvcodec);
	REPLACE_L(AVI_HACODEC, avi_hacodec);
	REPLACE_L(AVI_IS_INTERLEAVED, avi_is_interleaved);

	if (skin_rep_data_line_p->item_read != 0)
		REPLACE(READ, "read");
	else
		REPLACE(READ, "unread");

#undef REPLACE
#undef REPLACE_L
	return;
}

void skin_direct_replace_image_viewer(SKIN_T *skin, SKIN_REPLASE_IMAGE_VIEWER_DATA_T *image_viewer_info_p)
{
	skin_direct_replace_string(skin, SKIN_KEYWORD_SERVER_NAME, SERVER_NAME);

#define REPLACE_I(a, b) skin_direct_replace_string(skin, SKIN_KEYWORD_##a,\
			image_viewer_info_p->b)
	REPLACE_I(CURRENT_PATH, current_uri_name);
	REPLACE_I(CURRENT_PATH_LINK, current_uri_link);
	REPLACE_I(PARLENT_DIR_LINK, parent_directory_link );
	REPLACE_I(CURRENT_PAGE, now_page_str);
	REPLACE_I(LINE_TIMESTAMP, file_timestamp );
	REPLACE_I(LINE_FILE_DATE, file_timestamp_date );
	REPLACE_I(LINE_FILE_TIME, file_timestamp_time );
	REPLACE_I(LINE_FILE_SIZE, file_size_string );
	REPLACE_I(LINE_IMAGE_WIDTH, image_width );
	REPLACE_I(LINE_IMAGE_HEIGHT, image_height );
	REPLACE_I(IMAGE_VIEWER_WIDTH, image_viewer_width );
	REPLACE_I(IMAGE_VIEWER_HEIGHT, image_viewer_height );
	REPLACE_I(IMAGE_VIEWER_MODE, image_viewer_mode );
#undef REPLACE_I
}
