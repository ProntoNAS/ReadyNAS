// ==========================================================================
//code=EUC	tab=4
//
// wizd:	MediaWiz Server daemon.
//
// 		wizd_http.c
//											$Revision: 1.37 $
//											$Date: 2007/03/17 14:12:01 $
//
//	すべて自己責任でおながいしまつ。
//  このソフトについてVertexLinkに問い合わせないでください。
// ==========================================================================
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
# ifdef HAVE_W32API
#include <sys/cygwin.h>
# endif

#include "wizd.h"
#include "wizd_skin.h"




static int http_header_receive(int accept_socket, HTTP_RECV_INFO *http_recv_info);
static int http_file_check( HTTP_RECV_INFO *http_recv_info_p);
static int http_redirect_response(int accept_socket, HTTP_RECV_INFO *http_recv_info, char *location);
static int http_not_found_response(int accept_socket, HTTP_RECV_INFO *http_recv_info);
static int http_unauthorized_response(int accept_socket, HTTP_RECV_INFO *http_recv_info);

int line_receive(int accept_socket, unsigned char *line_buf_p, int line_max);

extern unsigned char *skin_file_read(unsigned char *read_filename, unsigned long *malloc_size);
extern int find_real_path(char *org_name, HTTP_RECV_INFO *recv_uri, char *work_filename);

# define SKIN_KEYWORD_MENU		"<wizd>menu</wizd>"
# define SKIN_KEYWORD_IMAGES	"../Data/Images"
# define SKIN_KEYWORD_DCIMAGES	"DCimages"
# define SKIN_KEYWORD_STYLESHEET "view_item_details"
# define SKIN_KEYWORD_FCOVER	"_f.jpg\""
# define SKIN_KEYWORD_BCOVER	"_b.jpg\""

int
http_find_and_replace(int accept_socket, HTTP_RECV_INFO *http_recv_info)
{
	char		*index_array[] = {"wizd.htm", "wizd.html", "index.htm", "index.html"};
	char		 wizd_buf[256];
	int			 i;
	SKIN_T 		*index_skin;
	char		*p;
	SKIN_REPLASE_GLOBAL_DATA_T	*skin_rep_data_global_p;
	extern void send_menu(int accept_socket, SKIN_REPLASE_GLOBAL_DATA_T *skin_rep_data_global_p, HTTP_RECV_INFO *http_recv_info_p, int skip, int special_view);

	for (i = 0; i < 4; i++) {
		if (find_real_path(index_array[i], http_recv_info, wizd_buf)) {
			if (strncasecmp(index_array[i], "wizd", 4) == 0) {
				debug_log_output("HTTP: got wizd.htm\n");

				http_send_ok_header(accept_socket, 0, NULL);
				index_skin = malloc(sizeof(SKIN_T));
				index_skin->buffer = skin_file_read(wizd_buf, &index_skin->buffer_size);
				skin_rep_data_global_p = skin_create_global_data(http_recv_info, 0);
				skin_direct_replace_global(index_skin, skin_rep_data_global_p);
				if ((p = strstr(index_skin->buffer, SKIN_KEYWORD_MENU)) != 0) {
					// add in the menu (mid_*.html) info
					send(accept_socket, index_skin->buffer, p - index_skin->buffer, 0);
					send_menu(accept_socket, skin_rep_data_global_p, http_recv_info, 0, 0);
					p += strlen(SKIN_KEYWORD_MENU);
					send(accept_socket, p, index_skin->buffer + strlen(index_skin->buffer) - p, 0);
				} else if ((p = strstr(index_skin->buffer, SKIN_KEYWORD_IMAGES)) != 0) {
					// add in the '/'
					send(accept_socket, index_skin->buffer, p - index_skin->buffer, 0);
					send(accept_socket, "/", 1, 0);
					p += strlen(SKIN_KEYWORD_IMAGES);
					send(accept_socket, p, index_skin->buffer + strlen(index_skin->buffer) - p, 0);
					
				} else
					skin_direct_send(accept_socket, index_skin);
				skin_close(index_skin);
				free( skin_rep_data_global_p );
			} else {
				debug_log_output("HTTP: got index.htm\n");
				strcpy(http_recv_info->send_filename, wizd_buf);
				http_file_response(accept_socket, http_recv_info);
			}

			return(1);
		}
	}

	return(0);
}

int
http_find_and_replace_html(int accept_socket, HTTP_RECV_INFO *http_recv_info)
{
	char		 wizd_buf[256];
	int			 i;
	int			 len;
	SKIN_T 		*index_skin;
	char		*p;
	SKIN_REPLASE_GLOBAL_DATA_T	*skin_rep_data_global_p;
	struct stat		dir_stat;
	int			found = 0;
	extern void send_menu(int accept_socket, SKIN_REPLASE_GLOBAL_DATA_T *skin_rep_data_global_p, HTTP_RECV_INFO *http_recv_info_p, int skip, int special_view);

	for (i = 0; i < global_param.num_aliases; i++) {
		len = strlen(global_param.alias_name[i]);
		if (strncmp(&(http_recv_info->recv_uri[1]), global_param.alias_name[i], len) == 0) {
			strcpy(wizd_buf, global_param.alias_path[i]);
			strcat(wizd_buf, http_recv_info->recv_uri + len + 1);
			if (stat(wizd_buf, &dir_stat) == 0) {
				found = 1;
				break;
			}
		}
	}

	if (found == 0) {
		p = strrchr(http_recv_info->recv_uri, '/');
		if (!p)
			return(0);
		p++;
		snprintf(wizd_buf, sizeof(wizd_buf), "%s/%s/%s", global_param.skin_root, global_param.skin_name, p);
		if (stat(wizd_buf, &dir_stat) != 0)
			return(0);
	}

	debug_log_output("HTTP: got wizd.htm\n");

	http_send_ok_header(accept_socket, 0, NULL);
	index_skin = malloc(sizeof(SKIN_T));
	index_skin->buffer = skin_file_read(wizd_buf, &index_skin->buffer_size);
	skin_rep_data_global_p = skin_create_global_data(http_recv_info, 0);
	sprintf(skin_rep_data_global_p->background_image, "back.jpg");
	skin_direct_replace_global(index_skin, skin_rep_data_global_p);
	if ((p = strstr(index_skin->buffer, SKIN_KEYWORD_MENU)) != 0) {
		// add in the menu (mid_*.html) info
		send(accept_socket, index_skin->buffer, p - index_skin->buffer, 0);
		send_menu(accept_socket, skin_rep_data_global_p, http_recv_info, 0, 0);
		p += strlen(SKIN_KEYWORD_MENU);
		send(accept_socket, p, index_skin->buffer + strlen(index_skin->buffer) - p, 0);

	}

	if (http_recv_info->default_file_type == TYPE_MOVIE) {
		// fix up the movie collector html page
		sprintf(wizd_buf, "%s\\Images\\", global_param.moviecollector);
		replase_character(index_skin->buffer, index_skin->buffer_size, wizd_buf, "/Images/");
		sprintf(wizd_buf, "%s\\Thumbnails\\", global_param.moviecollector);
		replase_character(index_skin->buffer, index_skin->buffer_size, wizd_buf, "/Thumbnails/");
		replase_character(index_skin->buffer, index_skin->buffer_size, global_param.moviecollector, "/MovieCollector");
		replase_character(index_skin->buffer, index_skin->buffer_size, SKIN_KEYWORD_DCIMAGES, "/MovieCollector/DCimages");
		replase_character(index_skin->buffer, index_skin->buffer_size, "view_item_", "/MovieCollector/view_item_");

		// for backward compatibility
		replase_character(index_skin->buffer, index_skin->buffer_size, "../Data/Images/", "/Images");
	} else if (http_recv_info->default_file_type == TYPE_MUSIC) {
		// fix up the music collector html page
		sprintf(wizd_buf, "%s\\Images\\", global_param.musiccollector);
		replase_character(index_skin->buffer, index_skin->buffer_size, wizd_buf, "/MusicImages/");
		sprintf(wizd_buf, "%s\\Thumbnails\\", global_param.musiccollector);
		replase_character(index_skin->buffer, index_skin->buffer_size, wizd_buf, "/MusicThumbnails/");
		replase_character(index_skin->buffer, index_skin->buffer_size, global_param.musiccollector, "/MusicCollector");
		replase_character(index_skin->buffer, index_skin->buffer_size, SKIN_KEYWORD_DCIMAGES, "/MusicCollector/DCimages");
		replase_character(index_skin->buffer, index_skin->buffer_size, "view_item_", "/MusicCollector/view_item_");
	}

	// printf("buffer %s\n", index_skin->buffer);
	skin_direct_send(accept_socket, index_skin);

	skin_close(index_skin);
	free( skin_rep_data_global_p );

	return(1);
}

void
http_get_url(char *uri, char *ret)
{
	FILE *fp;
	char buffer[256];
	int j;
			
	fp = fopen(uri, "r");
	if(fp != NULL) {
		debug_log_output("Scanning file for URL '%s'\n", uri);

		while(!feof(fp) && (NULL != fgets(buffer, sizeof(buffer), fp)) ) {
			if(strncasecmp(buffer, "URL=", 4) == 0) {
				// Strip trailing whitespace
				for(j=strlen(buffer)-1; (j>4) && isspace(buffer[j]); j--)
					buffer[j]=0;
				strcpy(ret, buffer+4);
				debug_log_output("Found URL '%s'\n", ret);
				break;
			}
		}
		fclose(fp);
	} else {
		debug_log_output("Unable to open file '%s'\n", uri);
	}
}

// **************************************************************************
// * サーバ HTTP処理部
// **************************************************************************
void 	server_http_process(int accept_socket)
{
	int					result;
	HTTP_RECV_INFO	http_recv_info;	//HTTP受信情報保存構造体
	int				i;
	int				flag_allow_user_agent_check;
	int				found;
	char 			buffer[WIZD_FILENAME_MAX];

	memset(&http_recv_info, 0, sizeof(http_recv_info));


	// ----------------------------------------
	// HTTP リクエストヘッダ受信
	// ----------------------------------------
	debug_log_output("HTTP Header receive start!\n");

	result = http_header_receive(accept_socket,  &http_recv_info);
	// printf("server_http_process got %s\n", http_recv_info.recv_uri);
	if ( result != 0 ) // エラーチェック
	{
		// エラーメッセージ
		debug_log_output("http_header_receive() Error. result=%d\n", result);
		// ソケットクローズ
		close(accept_socket);
		return;
	}
	debug_log_output("HTTP Header receive end!\n");

	debug_log_output("recv_uri:'%s'\n", http_recv_info.recv_uri);
	debug_log_output("user_agent:'%s'\n", http_recv_info.user_agent);

	debug_log_output("range_start_pos=%lld\n", http_recv_info.range_start_pos);
	debug_log_output("range_end_pos=%lld\n", http_recv_info.range_end_pos);

	if (http_recv_info.passwd[0]) {
		debug_log_output("http passwd:'%s'\n", http_recv_info.passwd);
	}

	// ==========================
	// = User-Agent チェック
	// ==========================

	flag_allow_user_agent_check = 0;

	if ( strlen(allow_user_agent[0].user_agent)  == 0 ) // User-Agnet指示が無し。
	{
		debug_log_output("user_agent: allow_user_agent No List. All Allow.");
		flag_allow_user_agent_check = 1; // OK
	}
	else
	{
		// User-Agent チェック実行
		for ( i=0; i<ALLOW_USER_AGENT_LIST_MAX; i++)
		{
			if ( strlen(allow_user_agent[i].user_agent)  == 0 )
			{
				break;
			}

			// 一致チェック
			debug_log_output("user_agent: Check[%d] '%s' in '%s'",i, allow_user_agent[i].user_agent, http_recv_info.user_agent);
			if ( strstr( http_recv_info.user_agent, allow_user_agent[i].user_agent ) != NULL )
			{
				debug_log_output("user_agent: '%s' OK.", allow_user_agent[i].user_agent );
				flag_allow_user_agent_check = 1; // 一致。OK
				break;
			}
		}
	}

	if (global_param.http_passwd[0] && !flag_allow_user_agent_check) {
		// パスワードが設定されて、正しければ、User-AgentチェックNGでもOK.
		char *pass, *p;

		pass = base64(global_param.http_passwd);
		if ((p = strchr(pass, '=')) != NULL) {
			*p = '\0';
		}
		if ((p = strchr(http_recv_info.passwd, '=')) != NULL) {
			*p = '\0';
		}

		if (!strcmp(pass, http_recv_info.passwd)) {
			// 一致
			flag_allow_user_agent_check = 1;
		} else {
			// 不一致
			// flag_allow_user_agent_check = 0;

			http_unauthorized_response(accept_socket, &http_recv_info);
			close(accept_socket);
			return ;
		}
	}

	// User-Agentチェック NGならば、ソケットクローズ。終了。
	if ( flag_allow_user_agent_check == 0 )
	{
		debug_log_output("allow_user_agent check. Deny. Socket close.");

		// ソケットクローズ
		close(accept_socket);
		return;
	}

	// クライアントがPCかどうか判断
	http_recv_info.flag_pc = (global_param.user_agent_pc[0]
		&& !strncmp(http_recv_info.user_agent, global_param.user_agent_pc
			, strlen(global_param.user_agent_pc))
	) ? 1 : 0;

	if(global_param.user_agent_pc[0]) {
		debug_log_output("Checking for PC user agent '%s'", global_param.user_agent_pc);
	}
	debug_log_output("flag_pc: %d", http_recv_info.flag_pc);

	// User-Agent
	// HD: 'Syabas/13-14-060414-04-IOD-234-000/04-IOD (uCOS-II v2.05;NOS;KA9Q; Res1280x720-HiColor; TV Res1920x1080; Browser Res1104x656-8bits; www.syabas.com mac_addr=00.a0.b0.65.51.31)'(186 byte)
	// SD: 'Syabas/13-14-060414-04-IOD-234-000/04-IOD (uCOS-II v2.05;NOS;KA9Q; Res624x416-HiColor; TV Res1920x1080; Browser Res624x416-8bits; www.syabas.com mac_addr=00.a0.b0.65.51.31)'

	http_recv_info.flag_hd = (strstr(http_recv_info.user_agent, "Res624x416") == NULL);

	if (!strncmp(http_recv_info.recv_uri, "/-.-", 4) || (http_recv_info.flag_pc && strcmp(http_recv_info.action, "allplay") == 0) || strstr(http_recv_info.recv_uri, "wizd_photolist.pls") != 0) {
		// proxy
		if (http_proxy_response(accept_socket, &http_recv_info) < 0) {
			http_not_found_response(accept_socket, &http_recv_info);
		}

		// ソケットクローズ
		close(accept_socket);
		return;
	}

	if (path_sanitize(http_recv_info.recv_uri, sizeof(http_recv_info.recv_uri)) == NULL) {
		// BAD REQUEST!
		debug_log_output("BAD REQUEST!");
		http_not_found_response(accept_socket, &http_recv_info);
		close(accept_socket);
		return;
	}
	debug_log_output("sanitized recv_uri: %s", http_recv_info.recv_uri);

	// ----------------------------------------
	// 受け取ったURIの拡張子がrename対象ならばrename
	// ----------------------------------------
	extension_del_rename(http_recv_info.recv_uri, sizeof(http_recv_info.recv_uri));

	// ============================
	// ファイルチェック
	//  種類に応じて分岐
	// ============================
	result = http_file_check(&http_recv_info);
	debug_log_output("http_file_check returns %d for %s, sendfilename %s, menu type is %d\n", result, http_recv_info.recv_uri, http_recv_info.send_filename, http_recv_info.menu_file_type);

	if (result != CGI_TYPE && !strcmp(http_recv_info.request_method, "POST")) {
		debug_log_output("BAD POST REQUEST.");
		http_not_found_response(accept_socket, &http_recv_info);
	}
	else if (result == -2) { // Directory without the trailing slash -> redirect
		sprintf(buffer, "%s/", http_recv_info.recv_uri);
		//printf("redirect to %s\n", buffer);
		http_redirect_response(accept_socket, &http_recv_info, buffer);
	} else if (result == URL_TYPE) {
		http_get_url(http_recv_info.send_filename, buffer);
		//printf("got URL %s, redirect to %s\n", http_recv_info.recv_uri, buffer);
		http_redirect_response(accept_socket, &http_recv_info, buffer);
		result = -2;
	} else if ( result < 0 ) // File not found
	{
		http_not_found_response(accept_socket, &http_recv_info);
	}
	else if ( result == GENERAL_FILE_TYPE ) // ファイル実体ならば、実体転送。
	{
		if ( global_param.flag_allow_delete && (strcasecmp(http_recv_info.action, "delete" ) == 0) )
		{
			// ----------------------------------------
			// Delete file
			// ----------------------------------------
			char path[WIZD_FILENAME_MAX];

			debug_log_output("Delete file start!\n");
			debug_log_output("unlink(%s)", http_recv_info.send_filename);
			unlink(http_recv_info.send_filename);
			// Remove any associated bookmarks if they exist
			snprintf(path, sizeof(path), "%s.wizd.bookmark", http_recv_info.send_filename);
			unlink(path);

			// Redirect to directory
			strncpy(path, http_recv_info.recv_uri, sizeof(path));
			cut_after_last_character(path, '/');
			strcat(path, "/");
			http_redirect_response(accept_socket, &http_recv_info, path);
			debug_log_output("Delete file end!\n");
		}
		else if ( strcasecmp(http_recv_info.action, "ImageView" ) == 0)
		{
			// ----------------------------------------
			// イメージファイルビューアー
			// ----------------------------------------
			debug_log_output("Image Viewer start!\n");
			http_image_viewer(accept_socket, &http_recv_info);
			debug_log_output("Image Viewer end!\n");
		}
		else if ( strcasecmp(http_recv_info.action, "SinglePlay" ) == 0)
		{
			// ----------------------------------------
			// Musicファイル 単独プレイ
			// ----------------------------------------
			debug_log_output("Single Play start!\n");
			http_music_single_play(accept_socket, &http_recv_info);
			debug_log_output("Single Play end!\n");
		}
#ifdef RESIZE_JPEG
		// actionに、Resizeが指示されている？
		else if ( strcasecmp(http_recv_info.action, "Resize.jpg" ) == 0)
		{
			// ----------------------------------------
			// JPEG Resize
			// ----------------------------------------
			debug_log_output("JPEG Resize start!\n");
			if(http_send_resized_jpeg(accept_socket, &http_recv_info) == 0) {
				// Failed to resize - send the original file as-is
				// (this can happen if we have the wrong file extension)
				http_file_response(accept_socket, &http_recv_info);
			}
			debug_log_output("JPEG Resize end!\n");
		}
#endif
		else if (strcmp(http_recv_info.option, "aviinfo") == 0)
		{
			debug_log_output("got aviinfo option\n");
			http_menu(accept_socket, &http_recv_info);
		}
		else if (strcmp(http_recv_info.option, "mp3info") == 0)
		{
			http_menu(accept_socket, &http_recv_info);
		}
		else // アクションに指定無し。
		{
			// ----------------------------------------
			// ファイルの実体
			// HTTPリクエストヘッダに従ってデータを返信。
			// ----------------------------------------
			found = 0;
			if (global_param.flag_use_index && strstr(http_recv_info.recv_uri, "wizd") != 0 && strstr(http_recv_info.recv_uri, ".htm") != 0) {
				if (http_find_and_replace_html(accept_socket, &http_recv_info)) {
					found = 1;
				}
			}

			if (!found) {
				debug_log_output("HTTP response start!\n");
				http_file_response(accept_socket, &http_recv_info);
				debug_log_output("HTTP response end!\n");
			}
		}
	}
	else if ( result == SVI_TYPE )
	{
		// ----------------------------------------
		// SVIファイル（；´Д｀）
		// SVIファイル処理してデータを返信
		// ----------------------------------------

		// actionに、SinglePlayが指示されている？
		if ( strcasecmp(http_recv_info.action, "SinglePlay" ) == 0)
		{
			// ----------------------------------------
			// Musicファイル 単独プレイ
			// ----------------------------------------
			debug_log_output("Single Play start!(SVI)\n");
			http_music_single_play(accept_socket, &http_recv_info);
			debug_log_output("Single Play end!(SVI)\n");
		}
		else // アクションに指定無し。
		{
			debug_log_output("HTTP joint file response start!\n");
			http_joint_file_response(accept_socket, &http_recv_info);
			debug_log_output("HTTP joint file response end!\n");
		}
	}
	else if ( result == PLW_UPL_TYPE )
	{
		// ---------------------------------------------
		// plw/uplファイル(`･ω･´)
		// リストファイルから、プレイリスト生成して返信
		// ---------------------------------------------
		debug_log_output("HTTP wizd play list create and response start!\n");
		http_listfile_to_playlist_create(accept_socket, &http_recv_info);
		debug_log_output("HTTP wizd play list create and response end!\n");
	}
	else if ( result == VOB_TYPE )
	{
		// ---------------------------------------------
		// vobファイル 連結
		// vobを連結して返信
		// ---------------------------------------------

		// actionに、SinglePlayが指示されている？
		if ( strcasecmp(http_recv_info.action, "SinglePlay" ) == 0)
		{
			// ----------------------------------------
			// Musicファイル 単独プレイ
			// ----------------------------------------
			debug_log_output("Single Play start!(VOB)\n");
			http_music_single_play(accept_socket, &http_recv_info);
			debug_log_output("Single Play end!(VOB)\n");
		}
		else // アクションに指定無し。
		{
			debug_log_output("HTTP vob file response start!\n");
			http_vob_file_response(accept_socket, &http_recv_info);
			debug_log_output("HTTP vob file response end!\n");
		}
	}
	else if ( result == CGI_TYPE )
	{
		// ---------------------------------------------
		// cgiファイル
		// cgiを実行して結果を返信
		// ---------------------------------------------
		debug_log_output("HTTP CGI response start!\n");
		http_cgi_response(accept_socket, &http_recv_info);
		debug_log_output("HTTP CGI response end!\n");
	}
	else if ( result == ISO_TYPE )
	{ // ISO file handling
		if ( (strcasecmp(http_recv_info.action, "IsoPlay" ) == 0) ||
			(strcasecmp(http_recv_info.action, "dvdplay" ) == 0) ||
			(strncmp(http_recv_info.action, "showchapters", 12 ) == 0))
		{
			debug_log_output("ISO playlist create start!\n");
			http_menu(accept_socket, &http_recv_info);
			debug_log_output("ISO playlist create end!\n");
		}
		else
		{
			// Start to stream the VOB
			debug_log_output("HTTP ISO file response start!\n");
			http_vob_file_response(accept_socket, &http_recv_info);
			debug_log_output("HTTP ISO file response end!\n");
		}
	}
	else
	{
		// ----------------------------------------
		// ディレクトリ
		// ----------------------------------------

		// actionに、OptionMenuが指示されている？
		if ( strcasecmp(http_recv_info.action, "OptionMenu" ) == 0)
		{
			debug_log_output("HTTP Option menu create.\n");
			http_option_menu(accept_socket, &http_recv_info);
			debug_log_output("HTTP Option menu end.\n");
		}
		else if ( strcasecmp(http_recv_info.action, "copy" ) == 0)
		{
			// --------------------------------------------
			// Copy the default playlist to this directory
			// --------------------------------------------
			char source[WIZD_FILENAME_MAX];
			char dest[WIZD_FILENAME_MAX];
			int fd_source, fd_dest;

			if(http_recv_info.default_file_type == TYPE_MUSICLIST) {
				snprintf(dest, sizeof(dest), "%s/%s", http_recv_info.send_filename, DEFAULT_MUSICLIST);
				snprintf(source, sizeof(source), "%s/%s/%s", global_param.skin_root, global_param.skin_name, DEFAULT_MUSICLIST);
			} else {
				snprintf(dest, sizeof(dest), "%s/%s", http_recv_info.send_filename, DEFAULT_PHOTOLIST);
				snprintf(source, sizeof(source), "%s/%s/%s", global_param.skin_root, global_param.skin_name, DEFAULT_PHOTOLIST);
			}
			duplex_character_to_unique(source, '/');
			duplex_character_to_unique(dest, '/');

			debug_log_output("Copying '%s' to '%s'", source, dest);

			// Copy the file
			fd_source = open(source, O_RDONLY);
			if(fd_source >= 0) {
				fd_dest = open(dest, O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE);
				if(fd_dest >= 0) {
					while( (i = read(fd_source, source, sizeof(source))) > 0 ) {
						write(fd_dest, source, i);
					}
					close(fd_dest);
				} else {
					debug_log_output("Failed to open destination file");
				}
				close(fd_source);
			} else {
				debug_log_output("Failed to open source file");
			}
			
			// Redirect to the directory
			strncpy(dest, http_recv_info.recv_uri, sizeof(dest));
			cut_after_last_character(dest, '/');
			strcat(dest, "/");
			http_redirect_response(accept_socket, &http_recv_info, dest);
			debug_log_output("Copy playlist end!\n");
		}
		else	// DIRECTORY_TYPE or {wizd,index}.htm{l}
		{
			int	found = 0;

			if (global_param.flag_use_index) {
				if (http_find_and_replace(accept_socket, &http_recv_info))
					found = 1;
			}

			if (!found) {
				debug_log_output("HTTP file menu create.\n");
				http_menu(accept_socket, &http_recv_info);
				debug_log_output("HTTP file menu end.\n");
			}
		}
	}

	// Clean shutdown
	shutdown(accept_socket, SHUT_WR);
 
	for(i=0; i<1 && (read(accept_socket, &result, sizeof(result))>0); i++);



	// ソケットクローズ
	close(accept_socket);

	return;
}

void
replace_question(char *p)
{
	while ((p = strchr(p, '?')) != NULL ) {
		*p = '&';
		p++;
	}
}

void
replace_str(char *str, char *subst, char new)
{
	char	buf[256];
	char	*p, *p2;
	int		 len, len2;

	p = strstr(str, subst);
	if (!p)
		return;

	len = p - str;
	memcpy(buf, str, len);
	buf[len] = new;
	len++;
	p += strlen(subst);

	while ((p2 = strstr(p, subst)) != NULL) {
		len2 = p2 - p;
		memcpy(buf + len, p, len2);
		buf[len + len2] = new;
		len += 1 + len2;
		p2 += strlen(subst);
		p = p2;
	}

	if (*p != '\0')
		strcpy(buf + len, p);

	debug_log_output("string was '%s', now '%s'\n", str, buf);
	strcpy(str, buf);
}

// **************************************************************************
// HTTPヘッダを受信して解析する。
//
// 処理するのはGETのみ。GET以外のメソッドが来たらエラー
// 今のところ、URIとuser_agent、Range、Hostを解析。
// URIは、URIデコードもやる。
//
//	return: 0 		正常終了
//	return: 0以外 	エラー
// **************************************************************************
static int http_header_receive(int accept_socket, HTTP_RECV_INFO *http_recv_info_p)
{
	int result = 0;
	char *p;
	int	recv_len;
	unsigned char	line_buf[1024];	// 大きめに。
	unsigned char 	work_buf[1024];
	unsigned char 	work_buf2[1024];
	unsigned char 	split1[1024];
	unsigned char 	split2[1024];
	int				got_width = 0;
	int				got_height = 0;

	int		ret;
	int		i;
	int		j;

	// ================================
	// １行づつ HTTPヘッダを受信
	// ================================
	for (i=0;;i++)
	{
		// 1行受信 実行。
		memset(line_buf, '\0', sizeof(line_buf));
		recv_len = line_receive(accept_socket, line_buf, sizeof(line_buf));

		// debug. 受信したヘッダ表示
		debug_log_output("'%s'(%d byte)\n", line_buf, recv_len );

		// 受信した内容をチェック。
		if ( i != 0 && recv_len == 0 ) // 空行検知。ヘッダ受信終了。
		{
			break;
		}
		else if ( recv_len < 0 ) // 受信失敗
		{
			return ( -1 );
		}

		// --------------------------
		// GETメッセージチェック
		// --------------------------
		if ( i == 0 ) // １行目のみチェック
		{
			debug_log_output("URI Check start.'%s'\n", line_buf);

			// GET/POSTある？
			if (strncmp(line_buf, "GET ", 4) && strncmp(line_buf, "POST ", 5))
			{
				debug_log_output("'GET' or 'POST' not found. error.");
				return ( -1 );
			}
			strncpy(http_recv_info_p->request_method, line_buf, sizeof(http_recv_info_p->request_method));
			cut_after_character(http_recv_info_p->request_method, ' ');

			// 最初のスペースまでを削除。
			cut_before_character(line_buf, ' ');

			// 次にスペースが出てきたところの後ろまでを削除。
			cut_after_character(line_buf, ' ');

			// ===========================
			// GETオプション部解析
			// ===========================

			// REQUEST_URI用・Proxy用に値を保存
			strncpy(http_recv_info_p->request_uri, line_buf, sizeof(http_recv_info_p->request_uri));
			//printf("got %s\n", http_recv_info_p->request_uri);

			// '?'が存在するかチェック。
			if ( (p = strchr(line_buf, '?')) != NULL )
			{

				// replace any other '?' with '&' fixes linkplayer problem
				replace_question(p + 1);

				// replace &amp; with &
				replace_str(p + 1, "&amp;", '&');

				strncpy(work_buf, line_buf, sizeof(work_buf));

				// '?'より前をカット
				cut_before_character(work_buf, '?' );
				debug_log_output("work_buf = '%s'", work_buf );

				while ( 1 )
				{
					memset(split1, '\0', sizeof(split1));
					memset(split2, '\0', sizeof(split2));

					uri_decode(split1, sizeof(split1), work_buf, sizeof(work_buf) );
					strcpy(work_buf, split1);

					// 最初に登場する'&'で分割
					ret = sentence_split(work_buf, '&', split1, split2 );
					if ( ret == 0 ) // 分割成功
					{
						strncpy(work_buf, split2, sizeof(work_buf));
					}
					else if (strlen(work_buf) > 0) // まだwork_bufに中身ある？
					{
						strncpy( split1, work_buf, sizeof(split1));
						strncpy( work_buf, "", sizeof(work_buf));
					}
					else // 処理終了
					{
						break;
					}

					// -------------------------------------
					// GETした内容 解析開始
					// 超安直。いいのかこんな比較で。
					// -------------------------------------

					// URIデコード
					// uri_decode(work_buf2, sizeof(work_buf2), split1, sizeof(split1) );
					strcpy(work_buf2, split1);

					// "page="あるか調査。
					if (strncasecmp( work_buf2, "startitem=", strlen("startitem=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
						{
							// 構造体に値を保存。
							http_recv_info_p->startitem = atoi(work_buf2);
						}

						continue;
					}

					if (strncasecmp( work_buf2, "page=", strlen("page=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
						{
							// 構造体に値を保存。
							http_recv_info_p->page = atoi(work_buf2);
						}

						continue;
					}

					// "off=" offset for mpg with chapter list
					if (strncasecmp( work_buf2, "off=", strlen("off=") ) == 0 )
					{
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
						{
							http_recv_info_p->off = atoll(work_buf2);
						}

						continue;
					}

					// "menupage="あるか調査。
					if (strncasecmp( work_buf2, "menupage=", strlen("menupage=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
						{
							// 構造体に値を保存。
							http_recv_info_p->menupage = atoi(work_buf2);
						}

						continue;
					}

					// "title=" DVD title selection
					if (strncasecmp( work_buf2, "title=", strlen("title=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
						{
							// 構造体に値を保存。
							http_recv_info_p->title = atoi(work_buf2);
						}

						continue;
					}

					if (strncasecmp( work_buf2, "maxwidth=", strlen("maxwidth=") ) == 0 )
					{
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
							global_param.max_jpeg_width = atoi(work_buf2);

						continue;
					}

					if (strncasecmp( work_buf2, "maxheight=", strlen("maxheight=") ) == 0 )
					{
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
							global_param.max_jpeg_height = atoi(work_buf2);

						continue;
					}

					if (strncasecmp( work_buf2, "width=", strlen("width=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
						{
							// 構造体に値を保存。
							global_param.target_jpeg_width = atoi(work_buf2);
							got_width = 1;
							if (!got_height)
								global_param.target_jpeg_height = 0;
						}

						continue;
					}

					if (strncasecmp( work_buf2, "height=", strlen("height=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
						{
							// 構造体に値を保存。
							global_param.target_jpeg_height = atoi(work_buf2);
							got_height = 1;
							if (!got_width)
								global_param.target_jpeg_width = 0;
						}

						continue;
					}

					// "action="あるか調査。
					if (strncasecmp( work_buf2, "action=", strlen("action=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');

						// 構造体に値を保存。
						strncpy(http_recv_info_p->action, work_buf2, sizeof(http_recv_info_p->action));
						continue;
					}

					if (strncasecmp( work_buf2, "enditem=", strlen("enditem=") ) == 0 )
					{
						cut_before_character(work_buf2, '=');
						if ( strlen(work_buf2) > 0 )
							http_recv_info_p->enditem = atoi(work_buf2);

						continue;
					}

					// "type=" allplay list type
					if (strncasecmp( work_buf2, "type=movie", strlen("type=movie") ) == 0 )
					{
						http_recv_info_p->default_file_type = TYPE_MOVIE;
						continue;
					}
					if (strncasecmp( work_buf2, "type=music", strlen("type=music") ) == 0 )
					{
						http_recv_info_p->default_file_type = TYPE_MUSIC;
						continue;
					}
					if (strncasecmp( work_buf2, "type=photo", strlen("type=photo") ) == 0 )
					{
						http_recv_info_p->default_file_type = TYPE_JPEG;
						continue;
					}
					if (strncasecmp( work_buf2, "type=soundtrack", strlen("type=soundtrack") ) == 0 )
					{
						http_recv_info_p->default_file_type = TYPE_MUSICLIST;
						continue;
					}
					if (strncasecmp( work_buf2, "type=slideshow", strlen("type=slideshow") ) == 0 )
					{
						http_recv_info_p->default_file_type = TYPE_PLAYLIST;
						continue;
					}
					
					// "option="あるか調査
					if (strncasecmp( work_buf2, "option=", strlen("option=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');

						// 構造体に値を保存。
						strncpy(http_recv_info_p->option, work_buf2, sizeof(http_recv_info_p->option));
						continue;
					}

					// "alias="あるか調査
					if (strncasecmp( work_buf2, "alias=", strlen("alias=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');

						// 構造体に値を保存。
						strncpy(http_recv_info_p->alias, work_buf2, sizeof(http_recv_info_p->alias));

						if (strncasecmp(http_recv_info_p->alias, "movie", strlen("movie")) == 0)
							http_recv_info_p->default_file_type = TYPE_MOVIE;
						else if (strncasecmp(http_recv_info_p->alias, "music", strlen("music")) == 0)
							http_recv_info_p->default_file_type = TYPE_MUSIC;
						else if (strncasecmp(http_recv_info_p->alias, "photo", strlen("photo")) == 0)
							http_recv_info_p->default_file_type = TYPE_JPEG;
						else
							http_recv_info_p->default_file_type = TYPE_UNKNOWN;

						continue;
					}

					if (strncasecmp( work_buf2, "lsearch=", strlen("lsearch=") ) == 0 )
					{
						cut_before_character(work_buf2, '=');

						strcpy(http_recv_info_p->lsearch, work_buf2);
						strcpy(http_recv_info_p->recv_uri, work_buf2);
						continue;
					}

					// "search=
					if (strncasecmp( work_buf2, "search", strlen("search") ) == 0 )
					{
						if (strncasecmp(work_buf2, "search_movie", strlen("search_movie")) == 0) {
							http_recv_info_p->search_type = TYPE_MOVIE;
							strcpy(http_recv_info_p->search_str, "_movie");
							http_recv_info_p->default_file_type = TYPE_MOVIE;
							// printf("search movie for ");
						} else if (strncasecmp(work_buf2, "search_music", strlen("search_music")) == 0) {
							http_recv_info_p->search_type = TYPE_MUSIC;
							strcpy(http_recv_info_p->search_str, "_music");
							http_recv_info_p->default_file_type = TYPE_MUSIC;
							// printf("search music for ");
						} else if (strncasecmp(work_buf2, "search_photo", strlen("search_photo")) == 0) {
							http_recv_info_p->search_type = TYPE_JPEG;
							strcpy(http_recv_info_p->search_str, "_photo");
							http_recv_info_p->default_file_type = TYPE_JPEG;
							// printf("search photo for ");
						} else if (strncasecmp(work_buf2, "search_all", strlen("search_all")) == 0) {
							http_recv_info_p->search_type = TYPE_UNKNOWN;
							strcpy(http_recv_info_p->search_str, "_all");
							http_recv_info_p->default_file_type = TYPE_UNKNOWN;
							// printf("search all for ");
						} else
							continue;

						cut_before_character(work_buf2, '=');

						strncpy(http_recv_info_p->search, work_buf2, sizeof(http_recv_info_p->search));
						if (http_recv_info_p->search[0] == '\0')
							// everything qualifies
							strcpy(http_recv_info_p->search, ".*");

						// printf("%s\n", http_recv_info_p->search);
						continue;
					}

					// "sort="あるか調査
					if (strncasecmp( work_buf2, "sort=", strlen("sort=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');

						// 構造体に値を保存。
						strncpy(http_recv_info_p->sort, work_buf2, sizeof(http_recv_info_p->sort));
						continue;
					}


					// "dvdopt="あるか調査
					if (strncasecmp( work_buf2, "dvdopt=", strlen("dvdopt=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');

						// 構造体に値を保存。
						strncpy(http_recv_info_p->dvdopt, work_buf2, sizeof(http_recv_info_p->dvdopt));
						continue;
					}

					// "focus="あるか調査
					if (strncasecmp( work_buf2, "focus=", strlen("focus=") ) == 0 )
					{
						// = より前を削除
						cut_before_character(work_buf2, '=');

						// 構造体に値を保存。
						strncpy(http_recv_info_p->focus, work_buf2, sizeof(http_recv_info_p->focus));
						continue;
					}




				}
			}

			debug_log_output("http_recv_info_p->page = '%d'", http_recv_info_p->page);
			debug_log_output("http_recv_info_p->title = '%d'", http_recv_info_p->title);
			debug_log_output("http_recv_info_p->action = '%s'", http_recv_info_p->action);
			debug_log_output("http_recv_info_p->option = '%s'", http_recv_info_p->option);
			debug_log_output("http_recv_info_p->dvdopt = '%s'", http_recv_info_p->dvdopt);

			// URIデコード
			cut_after_character(line_buf, '?');
			uri_decode(work_buf, sizeof(work_buf), line_buf, sizeof(line_buf) );
			strncpy(line_buf, work_buf, sizeof(line_buf));
			debug_log_output("URI(decoded):'%s'\n", line_buf);

			convert_language_code(line_buf, work_buf, sizeof(work_buf), CODE_AUTO, CODE_EUC);
			debug_log_output("URI(decoded,euc,FYI):'%s'\n", work_buf);


			// 構造体に保存
			if (http_recv_info_p->lsearch[0] == '\0')
				strncpy(http_recv_info_p->recv_uri, line_buf, sizeof(http_recv_info_p->recv_uri));

			continue;

		}

		// User-agent切り出し
		if ( strncasecmp(line_buf, HTTP_USER_AGENT, strlen(HTTP_USER_AGENT) ) == 0 )
		{
			// ':'より前を切る
			cut_before_character(line_buf, ':');
			cut_first_character(line_buf, ' ');

			// 構造体に保存
			strncpy( http_recv_info_p->user_agent, line_buf, sizeof(http_recv_info_p->user_agent));

			// Set the skin name based on user agent, if desired
			for(j=0; j<global_param.alternate_skin_count; j++) {
				debug_log_output("Checking for '%s'", global_param.alternate_skin_match[j]);
				if(strstr(line_buf, global_param.alternate_skin_match[j]) != NULL) {
					strcpy(global_param.skin_name, global_param.alternate_skin_name[j]);
					debug_log_output("User agent matches alternate skin '%s'", global_param.skin_name); 
					break;
				}
			}
			continue;
		}

		// Rangeあるかチェック
		if ( strncasecmp(line_buf, HTTP_RANGE,	strlen(HTTP_RANGE) ) == 0 )
		{
			debug_log_output("%s Detect.\n", HTTP_RANGE);
			// ':' より前を切る。
			cut_before_character(line_buf, ':');
			cut_first_character(line_buf, ' ');

			// recv_range にRangeの中身保存
			strncpy(http_recv_info_p->recv_range, line_buf, sizeof(http_recv_info_p->recv_range));

			// '=' より前を切る
			cut_before_character(line_buf, '=');


			// '-'で前後に分割。
			sentence_split(line_buf, '-', work_buf, work_buf2);

			debug_log_output("work_buf='%s'\n", work_buf);
			debug_log_output("work_buf2='%s'\n", work_buf2);

			// 値を文字列→数値変換
			http_recv_info_p->range_start_pos  = strtoull(work_buf, NULL, 10);

			if ( strlen(work_buf2) > 0 )
			{
				http_recv_info_p->range_end_pos = strtoull(work_buf2, NULL, 10);
			}


			debug_log_output("range_start_pos=%d\n", http_recv_info_p->range_start_pos);
			debug_log_output("range_end_pos=%d\n", http_recv_info_p->range_end_pos);

			continue;
		}

		// Hostあるかチェック
		if ( strncasecmp(line_buf, HTTP_HOST,	strlen(HTTP_HOST) ) == 0 )
		{
			debug_log_output("%s Detect.\n", HTTP_HOST);
			// ':' より前を切る。
			cut_before_character(line_buf, ':');
			cut_first_character(line_buf, ' ');

			strncpy(http_recv_info_p->recv_host, line_buf, sizeof(http_recv_info_p->recv_host));
			if(NULL == strchr(http_recv_info_p->recv_host, ':')) {
				debug_log_output("CLIENT BUG: Host header field was missing port number - fixing");
				snprintf(http_recv_info_p->recv_host + strlen(http_recv_info_p->recv_host), sizeof(http_recv_info_p->recv_host)-1, ":%d", global_param.server_port);
				debug_log_output("%s '%s'", HTTP_HOST, http_recv_info_p->recv_host);
			}

			continue;
		}

		if (strncasecmp(line_buf, HTTP_AUTHORIZATION, strlen(HTTP_AUTHORIZATION)) == 0) {
			debug_log_output("%s Detect.\n", HTTP_AUTHORIZATION);
			// ':' より前を切る。
			cut_before_character(line_buf, ':');
			cut_first_character(line_buf, ' ');
			if (strncmp(line_buf, "Basic ", 6)) {
				debug_log_output("received '%s', is not supported.", line_buf);
				continue;
			}

			strncpy(http_recv_info_p->passwd, line_buf + 6, sizeof(http_recv_info_p->passwd));

			continue;
		}

		// Content-Lengthあるかチェック
		if ( strncasecmp(line_buf, HTTP_CONTENT_LENGTH_STR,	strlen(HTTP_CONTENT_LENGTH_STR) ) == 0 )
		{
			debug_log_output("%s Detect.\n", HTTP_CONTENT_LENGTH_STR);
			// ':' より前を切る。
			cut_before_character(line_buf, ':');
			cut_first_character(line_buf, ' ');
			/* recv_content_length は わざと long です. */
			http_recv_info_p->recv_content_length = strtol(line_buf, NULL, 10);
			debug_log_output("Content-Length: %ld", http_recv_info_p->recv_content_length);
			continue;
		}

	}

	return result;
}


// **************************************************************************
// リクエストされたURIのファイルをチェック
// documet_rootと、skin置き場をセットで探す。
//
// Sets http_recv_info_p->file_type to one of the following:
//			-2: Directory without the slash
//			-1: File not found
//			 0: Normal file
//			 1: Directory
//			 2: SVI
//			 3: plw/upl
//			 4: tsv
//			 5: VOB video file
//			 6: CGI script
//			 7: ISO DVD image
// **************************************************************************
static int http_file_check( HTTP_RECV_INFO *http_recv_info_p)
{
	struct stat send_filestat;
	int result;
	int len;

	unsigned char	file_extension[16];

	debug_log_output("http_file_check() start.");

	// ---------------
	// 作業用変数初期化
	// ---------------
	memset(http_recv_info_p->send_filename, '\0', sizeof(http_recv_info_p->send_filename));
	memset(file_extension, '\0', sizeof(file_extension));

	// -------------------------
	// ファイルチェック
	// -------------------------

	// 要求パスのフルパス生成。
	if (strcmp(http_recv_info_p->recv_uri, "/") == 0) {
		strncpy(http_recv_info_p->send_filename, global_param.document_root, sizeof(http_recv_info_p->send_filename));
		strncat(http_recv_info_p->send_filename, http_recv_info_p->recv_uri, sizeof(http_recv_info_p->send_filename));
	} else {
		strncpy(http_recv_info_p->send_filename, global_param.skin_root, sizeof(http_recv_info_p->send_filename));
		strncat(http_recv_info_p->send_filename, global_param.skin_name, sizeof(http_recv_info_p->send_filename));
		strncat(http_recv_info_p->send_filename, http_recv_info_p->recv_uri, sizeof(http_recv_info_p->send_filename));
	}

	// Check if the URI is an alias
	// printf("uri1 is %s\n", http_recv_info_p->recv_uri);
	// printf("send_filename1 %s\n", http_recv_info_p->send_filename);
	if(global_param.num_aliases && (http_recv_info_p->recv_uri[0] == '/')) {
		int i,j,len, skip_stat;
		char *p;
		
		for(i=0; i<global_param.num_aliases; i++) {
			len = strlen(global_param.alias_name[i]);
			if ((strncmp(http_recv_info_p->recv_uri + 1, global_param.alias_name[i], len) == 0)
			   && (http_recv_info_p->recv_uri[len+1] == '/') ) {
			   // see code in set_thumb_file in wizd_menu.c
				p = strrchr(http_recv_info_p->recv_uri, '.');
				if (p && isdigit(p[1]) && isdigit(p[2]) && !p[3]) {
					p[0] = 0;
					p++;
					j = atoi(p);
					skip_stat = 1;
				} else {
					j = i;
					skip_stat = 0;
				}

				debug_log_output("substituting alias path '%s' for '%s'\n",
								  global_param.alias_path[j],
								  global_param.alias_name[j]);

				// The name matches an alias - substitute the alias path
				strncpy(http_recv_info_p->send_filename, global_param.alias_path[j], sizeof(http_recv_info_p->send_filename));
				strncat(http_recv_info_p->send_filename, http_recv_info_p->recv_uri+len+1, sizeof(http_recv_info_p->send_filename)-strlen(http_recv_info_p->send_filename));

				if (!skip_stat && stat(http_recv_info_p->send_filename, &send_filestat) != 0)
					continue;

				// Set the default allplay type, if it was not specified in the http request
				if(http_recv_info_p->default_file_type == TYPE_UNKNOWN) {
					http_recv_info_p->default_file_type = global_param.alias_default_file_type[j];
					debug_log_output("Setting default file type to %d\n",http_recv_info_p->default_file_type);
				} else {
					debug_log_output("Leaving default file type set at %d\n",http_recv_info_p->default_file_type);
				}
				break;
			}
		}
	}

	// fix up shortcuts
# ifdef HAVE_W32API
	len = strlen(http_recv_info_p->recv_uri);
	if (len > 4 && strcmp(&http_recv_info_p->recv_uri[len - 4], ".lnk") == 0) {
		char retPath1[256];
		char retPath2[256];
		char *p1, *p2;

		cygwin_conv_to_full_win32_path(http_recv_info_p->send_filename, retPath1);
		if (get_target(retPath1, retPath2) == 0) {
			cygwin_conv_to_full_posix_path(retPath2, http_recv_info_p->send_filename);

			p1 = strrchr(http_recv_info_p->recv_uri, '/');
			p1++;
			p2 = strrchr(http_recv_info_p->send_filename, '/');
			p2++;
			strcpy(p1, p2);
		}
	}
# endif

	// printf("send_filename2 %s\n", http_recv_info_p->send_filename);
	// printf("uri2 is %s\n\n", http_recv_info_p->recv_uri);

	// '/' が重なってるところの重複を排除。
	duplex_character_to_unique(http_recv_info_p->send_filename, '/');
	debug_log_output("http_recv_info_p->send_filename='%s'\n", http_recv_info_p->send_filename);


	// Return "File not found" unless we get a match
	http_recv_info_p->file_type = -1;

	// ------------------------------------------------------------
	// ファイルあるかチェック。
	// ------------------------------------------------------------
	result = stat(http_recv_info_p->send_filename, &send_filestat);
	debug_log_output("stat: result=%d, st_mode=0x%04X, S_ISREG=%d, S_ISDIR=%d size=%lld\n", 
		result, send_filestat.st_mode, S_ISREG(send_filestat.st_mode), S_ISDIR(send_filestat.st_mode), send_filestat.st_size );
	http_recv_info_p->file_size = send_filestat.st_size;

	if(result < 0)
	{
		debug_log_output("stat() error\n", result);

		// ----------------------------------------------------------------------------
		// もし、実体が存在しなかったら、skin置き場に変えてもう一度チェック
		// Skin置き場は実体ファイルのみ認める。
		// ----------------------------------------------------------------------------
# if 0
		debug_log_output("DocumentRoot not found. SkinDir Check.");

		debug_log_output("global_param.skin_root='%s'", global_param.skin_root);
		debug_log_output("global_param.skin_name='%s'", global_param.skin_name);


		// skin置き場にあるモノとして、フルパス生成。
		strncpy(http_recv_info_p->send_filename, global_param.skin_root, sizeof(http_recv_info_p->send_filename));
		strncat(http_recv_info_p->send_filename, global_param.skin_name, sizeof(http_recv_info_p->send_filename));
		strncat(http_recv_info_p->send_filename, http_recv_info_p->recv_uri, sizeof(http_recv_info_p->send_filename));

		// '/' が重なってるところの重複を排除。
		duplex_character_to_unique(http_recv_info_p->send_filename, '/');
		debug_log_output("SkinDir:http_recv_info_p->send_filename='%s'\n", http_recv_info_p->send_filename);


		// ------------------------------------------------------------
		// Skin置き場にファイルあるかチェック。
		// ------------------------------------------------------------
		result = stat(http_recv_info_p->send_filename, &send_filestat);
		debug_log_output("stat: result=%d, st_mode=%04X, S_ISREG=%d\n", result, send_filestat.st_mode, S_ISREG(send_filestat.st_mode));

		if( result < 0 ) {
			// Strip off the path, and try just the filename in the skin directory
			unsigned char *ptr = strrchr(http_recv_info_p->recv_uri, '/');
			if((ptr != NULL) && (ptr != http_recv_info_p->recv_uri)) {
				strncpy(http_recv_info_p->send_filename, global_param.skin_root, sizeof(http_recv_info_p->send_filename));
				strncat(http_recv_info_p->send_filename, global_param.skin_name, sizeof(http_recv_info_p->send_filename)-strlen(http_recv_info_p->send_filename));
				strncat(http_recv_info_p->send_filename, ptr, sizeof(http_recv_info_p->send_filename)-strlen(http_recv_info_p->send_filename));
				duplex_character_to_unique(http_recv_info_p->send_filename, '/');
				debug_log_output("SkinDir:http_recv_info_p->send_filename='%s'\n", http_recv_info_p->send_filename);
				result = stat(http_recv_info_p->send_filename, &send_filestat);
				debug_log_output("stat: result=%d, st_mode=%04X, S_ISREG=%d\n", result, send_filestat.st_mode, S_ISREG(send_filestat.st_mode));
			}
		}
# else
		strncpy(http_recv_info_p->send_filename, global_param.document_root, sizeof(http_recv_info_p->send_filename));
		strncat(http_recv_info_p->send_filename, http_recv_info_p->recv_uri, sizeof(http_recv_info_p->send_filename));
		debug_log_output("Checking docroot file %s\n", http_recv_info_p->send_filename);

		result = stat(http_recv_info_p->send_filename, &send_filestat);
		debug_log_output("stat: result=%d, st_mode=%04X, S_ISREG=%d\n", result, send_filestat.st_mode, S_ISREG(send_filestat.st_mode));
# endif

		// ファイル実体と検知。
		if ( ( result < 0 ) )
		{
			// -------------------------------------
			// File Not Found.
			// やっぱり、404にしよう。
			// -------------------------------------
			http_recv_info_p->file_type = -1; // Not found
			return(http_recv_info_p->file_type);
		}
	}


	// stat()の結果で分岐。
	if ( ( result == 0 ) && ( S_ISREG(send_filestat.st_mode) == 1 ) )
	{
		 // ファイル実体と検知
		debug_log_output("'%s' is File!!", http_recv_info_p->send_filename);
		debug_log_output("send_filestat.st_size= %lld\n", send_filestat.st_size);

		// Hack to overcome Linkplayer bug - it limits bookmark offset to (2^31)-1
		// so we remember the last bookmark served so we can recover the real offset
		if((http_recv_info_p->range_start_pos == 2147483647) && global_param.bookmark_threshold) {
			// Check for the real bookmark
			FILE *fp;
			off_t offset;
			char work_buf[WIZD_FILENAME_MAX];
			snprintf(work_buf, sizeof(work_buf), "%s.wizd.bookmark", 
				http_recv_info_p->send_filename);
			debug_log_output("Double-checking bookmark: '%s'", work_buf);
			fp = fopen(work_buf, "r");
			if(fp != NULL) {
				fgets(work_buf, sizeof(work_buf), fp);
				fclose(fp);
				offset = atoll(work_buf);
				if(offset > http_recv_info_p->range_start_pos) {
					debug_log_output("Detected LinkPlayer2 bookmark bug, replacing 'Range: %lld-' with 'Range: %lld-'",
						http_recv_info_p->range_start_pos, offset);
					http_recv_info_p->range_start_pos = offset;
				}
			}
		}


		// -------------------------------------------
		// ファイルの拡張子より、Content-type を決定
		// -------------------------------------------
		filename_to_extension(http_recv_info_p->send_filename, file_extension, sizeof(file_extension));
		debug_log_output("http_recv_info_p->send_filename='%s', file_extension='%s'\n", http_recv_info_p->send_filename, file_extension);

		// 拡張子から、mime_typeを導く。
		http_recv_info_p->menu_file_type = check_file_extension_to_mime_type(file_extension, http_recv_info_p->mime_type,  sizeof(http_recv_info_p->mime_type));
		// printf("got menu file type %d for %s\n", http_recv_info_p->menu_file_type, file_extension);

		// SVIファイルと実体ファイルで分岐
		if (( strcasecmp(file_extension, "svi") == 0 ) || ( strcasecmp(file_extension, "sv3") == 0 ))
		{
			http_recv_info_p->file_type = SVI_TYPE;	// sviファイル
		}
		else if (  ( http_recv_info_p->menu_file_type == TYPE_PLAYLIST )
				|| ( http_recv_info_p->menu_file_type == TYPE_MUSICLIST ))
			/*( strcasecmp(file_extension, "plw") == 0  ) ||
				 ( strcasecmp(file_extension, "pls") == 0  ) ||
				 ( strcasecmp(file_extension, "m3u") == 0  ) ||
				 ( strcasecmp(file_extension, "upl") == 0  ) )*/
		{
			http_recv_info_p->file_type = PLW_UPL_TYPE;	// plw/upl ファイル
		}
		else if (( strcasecmp(file_extension, "vob") == 0  )
			|| ( strcasecmp(file_extension, "ts") == 0  )
			|| ( strcasecmp(file_extension, "tp") == 0  ) )
		{
			// VOB_TYPE files combine files with increasing digits at the end of the filename
			// into one large virtual file
			http_recv_info_p->file_type = VOB_TYPE;	// vobファイル
		}
		else if ( strcasecmp(file_extension, "tsv") == 0  )
		{
			http_recv_info_p->file_type = TSV_TYPE;	// tsvファイル
		}
		else if ( strcasecmp(file_extension, "cgi") == 0  )
		{
			// CGIの実行が不許可なら、Not Found.
			http_recv_info_p->file_type = (global_param.flag_execute_cgi ? CGI_TYPE : -1 );	// cgiファイル
		}
		else if ( http_recv_info_p->menu_file_type == TYPE_ISO )
		{
			http_recv_info_p->file_type = ISO_TYPE;
		}
		else if ( strcasecmp(file_extension, "url") == 0 && http_recv_info_p->menu_file_type == TYPE_URL )
		{
			http_recv_info_p->file_type = URL_TYPE;
		}
		else
		{
			http_recv_info_p->file_type = GENERAL_FILE_TYPE;	// File実体
		}
	}
	else if ( ( result == 0 ) && ( S_ISDIR(send_filestat.st_mode) == 1 ) ) // ディレクトリ
	{
		int len;
		len = strlen(http_recv_info_p->recv_uri);
		if (len > 0 && http_recv_info_p->recv_uri[len - 1] != '/') {
			// '/' で終端していないディレクトリ要求の場合...
			http_recv_info_p->file_type = -2;
		}
		// ディレクトリと検知
		debug_log_output("'%s' is Dir!!", http_recv_info_p->send_filename);

		http_recv_info_p->file_type = DIRECTORY_TYPE ;	// ディレクトリ
	} else
		debug_log_output("%s not known\n", http_recv_info_p->send_filename);
	return( http_recv_info_p->file_type );
}






// **************************************************************************
// accept_socketから、１行(CRLFか、LF単独が現れるまで)受信
// CRLFは削除する。
// 受信したサイズをreturnする。
// **************************************************************************
int line_receive(int accept_socket, unsigned char *line_buf_p, int line_max)
{
	unsigned char byte_buf;
	int 	line_len;
	int		recv_len;

	unsigned char *line_buf_work_p;

	line_buf_work_p = line_buf_p;
	line_len = 0;

	// １行受信実行
	while ( 1 )
	{
		recv_len = recv(accept_socket, &byte_buf, 1, 0);
		if ( recv_len != 1 ) // 受信失敗チェック
		{
			//debug_log_output("line_receive: read_len == -1");
			return ( -1 );
		}

		// CR/LFチェック
		if ( byte_buf == '\r' )
		{
			continue;
		}
		else if ( byte_buf == '\n' )
		{
			break;
		}

		// バッファにセット
		*line_buf_work_p = byte_buf;
		line_buf_work_p++;
		line_len++;

		// printf("line_len=%d, buf='%s'\n", line_len, line_buf_p);

		// 受信バッファサイズチェック
		if ( line_len >= line_max)
		{
			// バッファオーバーフロー検知
			debug_log_output("line_buf over flow.");
			return line_len;
		}
	}

	return line_len;
}

static int http_redirect_response(int accept_socket, HTTP_RECV_INFO *http_recv_info, char *location)
{
	char buffer[WIZD_FILENAME_MAX];

	snprintf(buffer, sizeof(buffer),
		"HTTP/1.1 301 Found\r\n"
		"Location: %s\r\n"
		"\r\n", location
	);
	write(accept_socket, buffer, strlen(buffer)+1);

	return 0;
}

static int http_not_found_response(int accept_socket, HTTP_RECV_INFO *http_recv_info_p)
{
	char buffer[WIZD_FILENAME_MAX];

	if((strlen(http_recv_info_p->mime_type)==0) || (strcmp(http_recv_info_p->mime_type, "text/html") == 0)) {
	    snprintf(buffer, sizeof(buffer),
		HTTP_NOT_FOUND
		HTTP_CONNECTION
		HTTP_CONTENT_TYPE
		HTTP_END
		"<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n"
		"<BODY>File not found<P>\r\n"
		"%s\r\n"
		"</BODY></HTML>\r\n", http_recv_info_p->mime_type, http_recv_info_p->recv_uri
	    );
	} else {
	    snprintf(buffer, sizeof(buffer),
			HTTP_NOT_FOUND
			HTTP_CONNECTION
			HTTP_CONTENT_TYPE
			HTTP_END, "text/plain" );
	}
	debug_log_output("Not found: %s\n%s", http_recv_info_p->recv_uri, buffer);
	write(accept_socket, buffer, strlen(buffer)+1);

	return 0;

/*
	return http_redirect_response(accept_socket, http_recv_info_p, "/");
*/
}

static int http_unauthorized_response(int accept_socket, HTTP_RECV_INFO *http_recv_info)
{
	char buffer[WIZD_FILENAME_MAX];

	snprintf(buffer, sizeof(buffer),
		"HTTP/1.1 401 Authorization Required\r\n"
		"WWW-Authenticate: Basic realm=NEED AUTHENTICATE!\r\n"
		"\r\n"
	);
	debug_log_output("unauthorized response.");
	write(accept_socket, buffer, strlen(buffer)+1);

	return 0;
/*
	return http_redirect_response(accept_socket, http_recv_info, "/");
*/
}
