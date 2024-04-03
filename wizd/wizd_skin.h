#ifndef _WIZD_SKIN_H_
#define _WIZD_SKIN_H_

#include "wizd.h"

#define MAX_SKIN_FILESIZE (1024*16)

// ****************
// SKIN_T
// ****************
typedef struct _skin_t {
	char *buffer;
	unsigned long buffer_size;
} SKIN_T;


typedef struct _skin_mapping {
	int filetype;
	char *skin_filename;
} SKIN_MAPPING_T;


// スキン置換用データ（グローバル）
typedef struct  {
	unsigned char	current_path_name[WIZD_FILENAME_MAX];	// 現パス 表示用(文字コード調整済み)

	unsigned char	recv_host[256];
	unsigned char	current_directory_name[WIZD_FILENAME_MAX];	// 現ディレクトリ 表示用(文字コード調整済み)
	unsigned char	current_directory_link[WIZD_FILENAME_MAX];	// 現ディレクトリ Link用（URIエンコード済み）
	unsigned char	current_directory_absolute[FILENAME_MAX];	// 現ディレクトリLink用（URIエンコード済み）
	unsigned char	current_directory_link_no_param[WIZD_FILENAME_MAX];	// 現ディレクトリ Link用（URIエンコード済み）
	unsigned char	current_directory_link_no_sort[WIZD_FILENAME_MAX];
	unsigned char	current_directory_link_no_option[WIZD_FILENAME_MAX];
	unsigned char	current_directory_link_no_dvdopt[WIZD_FILENAME_MAX];

	unsigned char	parent_directory_name[WIZD_FILENAME_MAX];	// 親ディレクトリ表示用
	unsigned char	parent_directory_link[WIZD_FILENAME_MAX];	// 親ディレクトリLink用（URIエンコード済み）

	unsigned char	file_num_str[16];	// 現ディレクトリのファイル数表示用
	unsigned char	now_page_str[16];	// 現在のページ番号表示用
	unsigned char	max_page_str[16];	// 最大ページ番号表示用

	unsigned char	start_file_num_str[16];	// 現在ページの表示開始ファイル番号表示用
	unsigned char	end_file_num_str[16];	// 現在ページの表示開始ファイル番号表示用

	unsigned char	next_page_str[16];	// 次のページ(無ければmax_page)表示用
	unsigned char	prev_page_str[16];	// 一つ前のページ（無ければ 1)表示用

	unsigned char	focus[64];			// BODYタグ用 onloadset="$focus"

	unsigned char	default_photolist[WIZD_FILENAME_MAX];
	unsigned char	default_musiclist[WIZD_FILENAME_MAX];

	int		filename_length_max;

	int		stream_files;	// 再生可能ファイル数
	int		photo_files;
	int		music_files;
	int		stream_dirs;

	int		columns;
	int		items_per_page;
	int		now_page;		// Present page
	int		max_page;		// 最大ページ番号
	int		now_page_line;	// number of lines
	int		start_file_num;	// start file number of the page
	int		end_file_num;	// 現在ページの表示終了ファイル番号

	int		next_page;		// next page if not max page
	int		prev_page;		// 一つ前のページ（無ければ 1)

	int		delete_mode;		// When true, allow deleting files from client

	// 隠しディレクトリ情報
	unsigned char	secret_dir_link_html[512];

	// クライアントがPCかどうか
	int		flag_pc;

	// True when the client is using HD resolution
	int		flag_hd;

	char		default_dir_type[32];
	unsigned char 	request_uri[WIZD_FILENAME_MAX];

	char			background_image[255];

} SKIN_REPLASE_GLOBAL_DATA_T;


// スキン置換用データ。（ファイル）
typedef struct  {
	int				menu_file_type;			// ファイルの種類

	unsigned char	file_name[255];			// ファイル名表示用(文字コード調整済み)
	unsigned char	file_name_no_ext[255];	// 拡張子無しファイル名表示用(文字コード調整済み)
	unsigned char	file_extension[16];	// 拡張子のみ(文字コード調整済み)

	unsigned char	file_uri_link[WIZD_FILENAME_MAX];	// ファイルへのLink(URIエンコード済み)
	unsigned char	chapter_link[WIZD_FILENAME_MAX];
	unsigned char	chapter_str[WIZD_FILENAME_MAX];

	unsigned char	file_timestamp[32];		// タイムスタンプ表示用
	unsigned char	file_timestamp_date[32];	// タイムスタンプ表示用 日付のみ
	unsigned char	file_timestamp_time[32];	// タイムスタンプ表示用 日時のみ
	unsigned char	file_duration[32];	// タイムスタンプ表示用 日時のみ

	unsigned char	file_size_string[32];	// ファイルサイズ表示用

	unsigned char	svi_info_data[SVI_INFO_LENGTH];	// SVI ファイル情報
	unsigned char	svi_rec_time_data[32];			// SVI録画時間

	unsigned char	tvid_string[16];	// TVID表示用
	unsigned char	vod_string[32];		// vod="0" or vod="playlist"  必要に応じて付く
	int				row_num;		// 行番号

	unsigned char	image_width[16];	// 画像データ 横幅
	unsigned char	image_height[16];	// 画像データ 高さ


	// MP3 ID3v1 タグ情報
	unsigned char	mp3_id3v1_flag;			// MP3 タグ 存在フラグ
	unsigned char	mp3_id3v1_title[128];	// MP3 曲名
	unsigned char	mp3_id3v1_album[128];	// MP3 アルバム名
	unsigned char	mp3_id3v1_artist[128];	// MP3 アーティスト
	unsigned char	mp3_id3v1_year[128];		// MP3 制作年度
	unsigned char	mp3_id3v1_comment[128];	// MP3 コメント

	unsigned char	mp3_id3v1_title_info[128*4];			// MP3 曲名[アルバム名/アーティスト] まとめて表示
	unsigned char	mp3_id3v1_title_info_limited[128*4];	// MP3 曲名[アルバム名/アーティスト] まとめて表示(字数制限あり)

	unsigned char	mp3_id3v1_genre[128];
	unsigned char	mp3_id3v1_bitrate[128];
	unsigned char	mp3_id3v1_stereo[128];
	unsigned char	mp3_id3v1_frequency[128];
	unsigned char	mp3_id3v1_track[128];

	unsigned char	avi_fps[16];
	unsigned char	avi_duration[32];
	unsigned char	avi_vcodec[128];
	unsigned char	avi_acodec[128];
	unsigned char	avi_hvcodec[128];
	unsigned char	avi_hacodec[128];
	unsigned char	avi_is_interleaved[32];

	char			file_image[255];

	char			full_pathname[255];

	int				is_current_page;
	unsigned char	info_link[WIZD_FILENAME_MAX];
	unsigned char	html_link[WIZD_FILENAME_MAX];

	int				item_read;

} SKIN_REPLASE_LINE_DATA_T;



// ImageViewer 置換用データ
typedef struct  {

	unsigned char	current_uri_name[WIZD_FILENAME_MAX];	// 現URI 表示用(文字コード調整済み)
	unsigned char	current_uri_link[WIZD_FILENAME_MAX];	// 現URI Link用（URIエンコード済み）

	unsigned char	parent_directory_link[WIZD_FILENAME_MAX];	// 親ディレクトリLink用（URIエンコード済み）

	unsigned char	now_page_str[16];	// 現在のページ番号表示用

	unsigned char	file_timestamp[32];			// タイムスタンプ表示用(日時)
	unsigned char	file_timestamp_date[32];	// タイムスタンプ表示用(日付のみ)
	unsigned char	file_timestamp_time[32];		// タイムスタンプ表示用(時刻のみ)
	unsigned char	file_size_string[32];		// ファイルサイズ表示用

	unsigned char	image_width[16];			// 画像データ 横幅
	unsigned char	image_height[16];			// 画像データ 高さ

	unsigned char	image_viewer_width[16];		// 画像データ 表示横幅
	unsigned char	image_viewer_height[16];	// 画像データ 表示高さ

	unsigned char	image_viewer_mode[16];		// 表示モード

} SKIN_REPLASE_IMAGE_VIEWER_DATA_T;

void replase_skin_grobal_data(unsigned char *menu_work_p, int menu_work_buf_size, SKIN_REPLASE_GLOBAL_DATA_T *skin_rep_data_global_p);
void replase_skin_line_data(unsigned char *menu_work_p, int menu_work_buf_size, SKIN_REPLASE_LINE_DATA_T *skin_rep_data_line_p);

SKIN_T *skin_open(char *filename);
void skin_close(SKIN_T *skin);
void skin_read_config(char *filename);
unsigned char* skin_get_string(SKIN_T *skin);
void skin_direct_replace_string(SKIN_T *skin, unsigned char *orig, unsigned char *str);
void skin_direct_replace_format(SKIN_T *skin, unsigned char *orig, unsigned char *fmt, ...);
void skin_direct_replace_global(SKIN_T *skin, SKIN_REPLASE_GLOBAL_DATA_T *rep_p);
void skin_direct_cut_enclosed_words(SKIN_T *skin, unsigned char *s, unsigned char *e);
int skin_direct_send(int fd, SKIN_T *skin);
;
void replase_skin_grobal_data(unsigned char *menu_work_p, int menu_work_buf_size, SKIN_REPLASE_GLOBAL_DATA_T *skin_rep_data_global_p);
void replase_skin_line_data(unsigned char *menu_work_p, int menu_work_buf_size, SKIN_REPLASE_LINE_DATA_T *skin_rep_data_line_p);

void skin_direct_replace_image_viewer(SKIN_T *skin, SKIN_REPLASE_IMAGE_VIEWER_DATA_T *image_viewer_info_p);
SKIN_REPLASE_GLOBAL_DATA_T *skin_create_global_data(HTTP_RECV_INFO *http_recv_info_p, int file_num);

// --------------------------------------------------------------------------

#define		SKIN_MENU_CONF						"wizd_skin.conf"

#define		SKIN_MENU_HEAD_HTML					"head.html"
#define		SKIN_MENU_TAIL_HTML					"tail.html"

#define		SKIN_MENU_THUMB_HEAD_HTML				"thumb_head.html"
#define		SKIN_MENU_THUMB_TAIL_HTML				"thumb_tail.html"

#define     SKIN_MENU_MIDHEAD_HTML                  "midhead.html"
#define     SKIN_MENU_THUMB_MIDHEAD_HTML            "thumb_midhead.html"
#define     SKIN_MENU_MID_LINE_HTML                 "line_middle.html"
#define     SKIN_MENU_MIDTAIL_HTML                  "midtail.html"
#define     SKIN_MENU_THUMB_MIDTAIL_HTML            "thumb_midtail.html"

#define     SKIN_MENU_NAVHEAD_HTML                  "navhead.html"
#define     SKIN_MENU_NAV_LINE_HTML            		"line_nav.html"
#define     SKIN_MENU_NAVTAIL_HTML            		"navtail.html"

#define		SKIN_DELETE_HEAD_HTML					"delete_head.html"
#define		SKIN_DELETE_TAIL_HTML					"delete_tail.html"
#define		SKIN_DELETE_CONFIRM_HTML				"delete_confirm.html"

#define     SKIN_MENU_ALBUM_HTML            		"line_album.html"

#ifdef NEED_SKIN_MAPPING_DEFINITION
SKIN_MAPPING_T skin_mapping[] = {
	{TYPE_UNKNOWN,		"unknown"},
	{TYPE_DIRECTORY,	"dir"},
	{TYPE_VIDEO_TS,	"videots"},
	{TYPE_PSEUDO_DIR,	"pseudo"},
	{TYPE_MOVIE,		"movie"},
	{TYPE_PLAYLIST,		"playlist"},
	{TYPE_MUSIC,		"music"},
	{TYPE_MUSICLIST,	"music"},
	{TYPE_IMAGE,		"image"},
	{TYPE_DOCUMENT,		"document"},
	{TYPE_SVI,		"svi"},
	{TYPE_JPEG,		"jpeg"},
	{TYPE_URL,		"url"},
	{TYPE_ISO,		"iso"},
	{TYPE_DELETE,		"delete"},
	{TYPE_CHAPTER,		"chapter"},
	{TYPE_ROW,		"row"},
	{TYPE_MP3INFO,		"mp3info"},
	{TYPE_AVIINFO,		"aviinfo"},
	{-1,			NULL},
};
#else
extern SKIN_MAPPING_T skin_mapping[];
// extern, but never be used? :p
#endif

#define		SKIN_KEYWORD_SERVER_ADDRESS		"<!--WIZD_INSERT_SERVER_ADDRESS-->"
#define		SKIN_KEYWORD_SERVER_NAME		"<!--WIZD_INSERT_SERVER_NAME-->"		// サーバ名＆バージョン。表示用
#define		SKIN_KEYWORD_CURRENT_PATH		"<!--WIZD_INSERT_CURRENT_PATH-->"		// 現PATH。表示用
#define		SKIN_KEYWORD_CURRENT_DIR_NAME	"<!--WIZD_INSERT_CURRENT_DIR_NAME-->"		// 現ディレクトリ名。表示用
#define		SKIN_KEYWORD_CURRENT_DATE		"<!--WIZD_INSERT_CURRENT_DATE-->"				// 日付表示用
#define		SKIN_KEYWORD_CURRENT_TIME		"<!--WIZD_INSERT_CURRENT_TIME-->"				// 時刻表示用

#define		SKIN_KEYWORD_PARLENT_DIR_LINK	"<!--WIZD_INSERT_PARENT_DIR_LINK-->"	// 親ディレクトリ。LINK用 URIエンコード済み
#define		SKIN_KEYWORD_PARLENT_DIR_NAME	"<!--WIZD_INSERT_PARENT_DIR_NANE-->"	// 親ディレクトリ。表示用

#define		SKIN_KEYWORD_CURRENT_PATH_LINK	"<!--WIZD_INSERT_CURRENT_PATH_LINK-->"	// 現PATH。LINK用。URIエンコード済み
#define		SKIN_KEYWORD_CURRENT_PATH_FULL_LINK	"<!--WIZD_INSERT_CURRENT_PATH_FULL_LINK-->"
#define		SKIN_KEYWORD_CURRENT_PATH_LINK_NO_PARAM	"<!--WIZD_INSERT_CURRENT_PATH_LINK_NO_PARAM-->"	// 現PATH。LINK用。URIエンコード済み
#define		SKIN_KEYWORD_CURRENT_PATH_LINK_NO_SORT		"<!--WIZD_INSERT_CURRENT_PATH_LINK_NO_SORT-->"
#define		SKIN_KEYWORD_CURRENT_PATH_LINK_DVDOPT		"<!--WIZD_INSERT_CURRENT_PATH_LINK_DVDOPT-->"
#define		SKIN_KEYWORD_CURRENT_PATH_LINK_NO_DVDOPT	"<!--WIZD_INSERT_CURRENT_PATH_LINK_NO_DVDOPT-->"
#define		SKIN_KEYWORD_CURRENT_PATH_LINK_NO_OPTION	"<!--WIZD_INSERT_CURRENT_PATH_LINK_NO_OPTION-->"
#define		SKIN_KEYWORD_CURRENT_REQUEST_URI	"<!--WIZD_INSERT_CURRENT_URI-->"


#define		SKIN_KEYWORD_CURRENT_PAGE		"<!--WIZD_INSERT_CURRENT_PAGE-->"		// 現在のページ
#define		SKIN_KEYWORD_MAX_PAGE			"<!--WIZD_INSERT_MAX_PAGE-->"			// 全ページ数
#define		SKIN_KEYWORD_NEXT_PAGE			"<!--WIZD_INSERT_NEXT_PAGE-->"			// 次のページ
#define		SKIN_KEYWORD_PREV_PAGE			"<!--WIZD_INSERT_PREV_PAGE-->"			// 前のページ
#define		SKIN_KEYWORD_FILE_NUM			"<!--WIZD_INSERT_FILE_NUM-->"			// ディレクトリ存在ファイル数
#define		SKIN_KEYWORD_START_FILE_NUM		"<!--WIZD_INSERT_START_FILE_NUM-->"		// 次のページ
#define		SKIN_KEYWORD_END_FILE_NUM		"<!--WIZD_INSERT_END_FILE_NUM-->"		// 前のページ

#define		SKIN_KEYWORD_ONLOADSET_FOCUS		"<!--WIZD_INSERT_ONLOADSET_FOCUS-->"		// BODYタグ用 onloadset="$focus"

#define		SKIN_KEYWORD_CLIENT_CHARSET		"<!--WIZD_INSERT_CLIENT_CHARSET-->"		// クライアントの漢字コード

#define		SKIN_KEYWORD_DEFAULT_PHOTOLIST	"<!--WIZD_INSERT_DEFAULT_PHOTOLIST-->"
#define		SKIN_KEYWORD_DEFAULT_MUSICLIST	"<!--WIZD_INSERT_DEFAULT_MUSICLIST-->"


#define		SKIN_KEYWORD_BACKGROUND			"<!--WIZD_INSERT_BACKGROUND-->"

#define		SKIN_KEYWORD_LINE_FULL_PATHNAME	    "<!--WIZD_INSERT_LINE_FULL_PATHNAME-->"
#define		SKIN_KEYWORD_LINE_FILE_NAME			"<!--WIZD_INSERT_LINE_FILE_NAME-->"			// ファイル名 表示用
#define		SKIN_KEYWORD_LINE_FILE_NAME_NO_EXT	"<!--WIZD_INSERT_LINE_FILE_NAME_NO_EXT-->"	// ファイル名(拡張子無し) 表示用
#define		SKIN_KEYWORD_LINE_FILE_EXT			"<!--WIZD_INSERT_LINE_FILE_EXT-->"			// ファイル拡張子 表示用
#define		SKIN_KEYWORD_LINE_FILE_LINK			"<!--WIZD_INSERT_LINE_FILE_LINK-->"			// ファイル名 リンク用 URIエンコード
#define		SKIN_KEYWORD_LINE_CHAPTER_LINK		"<!--WIZD_INSERT_LINE_CHAPTER_LINK-->"
#define		SKIN_KEYWORD_LINE_CHAPTER_STR		"<!--WIZD_INSERT_LINE_CHAPTER_STR-->"
/* XXX */
#define		SKIN_KEYWORD_LINE_FILE_IMAGE		"<!--WIZD_INSERT_LINE_FILE_IMAGE-->"			// ファイル名 表示用

#define		SKIN_KEYWORD_LINE_INFO_LINK		    "<!--WIZD_INSERT_LINE_INFO_LINK-->"			// ファイル名 表示用

#define		SKIN_KEYWORD_LINE_HTML_LINK		    "<!--WIZD_INSERT_LINE_HTML_LINK-->"			// ファイル名 表示用
/* XXX */

#define		SKIN_KEYWORD_LINE_TIMESTAMP		"<!--WIZD_INSERT_LINE_TIMESTAMP-->"		// タイムスタンプ 日時(YYYY/MM/DD HH:MM) 表示用

#define		SKIN_KEYWORD_LINE_FILE_DATE		"<!--WIZD_INSERT_LINE_FILE_DATE-->"		// タイムスタンプ 日付のみ(YYYY/MM/DD) 表示用
#define		SKIN_KEYWORD_LINE_FILE_TIME		"<!--WIZD_INSERT_LINE_FILE_TIME-->"		// タイムスタンプ 時刻のみ(HH:MM) 表示用
#define		SKIN_KEYWORD_LINE_FILE_DURATION		"<!--WIZD_INSERT_LINE_FILE_DURATION-->"		// タイムスタンプ 時刻のみ(HH:MM) 表示用


#define		SKIN_KEYWORD_LINE_COLUMN_NUM	"<!--WIZD_INSERT_LINE_COLUMN_NUM-->"	// 行番号
#define		SKIN_KEYWORD_LINE_ROW_NUM	"<!--WIZD_INSERT_LINE_ROW_NUM-->"	// 行番号

#define		SKIN_KEYWORD_LINE_TVID			"<!--WIZD_INSERT_LINE_TVID-->"			// TVID

#define		SKIN_KEYWORD_LINE_FILE_VOD		"<!--WIZD_INSERT_LINE_FILE_VOD-->"		// vod="0"  必要に応じて付く。
#define		SKIN_KEYWORD_LINE_FILE_SIZE		"<!--WIZD_INSERT_LINE_FILE_SIZE-->"		// ファイルサイズ 表示用

#define		SKIN_KEYWORD_LINE_SVI_INFO		"<!--WIZD_INSERT_LINE_SVI_INFO-->"		// SVIファイルから読んだ情報 表示用
#define		SKIN_KEYWORD_LINE_SVI_REC_TIME	"<!--WIZD_INSERT_LINE_SVI_REC_TIME-->"	// SVIファイルの録画時間

#define		SKIN_KEYWORD_LINE_IMAGE_WIDTH	"<!--WIZD_INSERT_LINE_IMAGE_WIDTH-->"	// 画像の横幅
#define		SKIN_KEYWORD_LINE_IMAGE_HEIGHT	"<!--WIZD_INSERT_LINE_IMAGE_HEIGHT-->"	// 画像の高さ

#define		SKIN_KEYWORD_SECRET_DIR_LINK	"<!--WIZD_INSERT_SECRET_DIR_LINK-->"	// 隠しディレクトリ


#define		SKIN_KEYWORD_LINE_MP3TAG_TITLE		"<!--WIZD_INSERT_LINE_MP3TAG_TITLE-->"		// MP3タグ タイトル
#define		SKIN_KEYWORD_LINE_MP3TAG_ALBUM		"<!--WIZD_INSERT_LINE_MP3TAG_ALBUM-->"		// MP3タグ アルバム名
#define		SKIN_KEYWORD_LINE_MP3TAG_ARTIST		"<!--WIZD_INSERT_LINE_MP3TAG_ARTIST-->"		// MP3タグ アーティスト
#define		SKIN_KEYWORD_LINE_MP3TAG_YEAR		"<!--WIZD_INSERT_LINE_MP3TAG_YEAR-->"		// MP3タグ 制作年度
#define		SKIN_KEYWORD_LINE_MP3TAG_COMMENT	"<!--WIZD_INSERT_LINE_MP3TAG_COMMENT-->"	// MP3タグ コメント

#define		SKIN_KEYWORD_LINE_MP3TAG_GENRE	    "<!--WIZD_INSERT_LINE_MP3TAG_GENRE-->"	// MP3タグ コメント
#define		SKIN_KEYWORD_LINE_MP3TAG_BITRATE	"<!--WIZD_INSERT_LINE_MP3TAG_BITRATE-->"	// MP3タグ コメント
#define		SKIN_KEYWORD_LINE_MP3TAG_STEREO		"<!--WIZD_INSERT_LINE_MP3TAG_STEREO-->"	// MP3タグ コメント
#define		SKIN_KEYWORD_LINE_MP3TAG_FREQ		"<!--WIZD_INSERT_LINE_MP3TAG_FREQ-->"	// MP3タグ コメント
#define		SKIN_KEYWORD_LINE_MP3TAG_TRACK		"<!--WIZD_INSERT_LINE_MP3TAG_TRACK-->"	// MP3タグ コメント

#define		SKIN_KEYWORD_LINE_MP3TAG_TITLE_INFO	"<!--WIZD_INSERT_LINE_MP3TAG_TITLE_INFO-->"	// MP3タグ タイトル[アルバム名/アーティスト] 表示(menu_filename_length_maxによる制限も効く)

#define		SKIN_KEYWORD_LINE_MP3TAG_TITLE_INFO_LIMITED	"<!--WIZD_INSERT_LINE_MP3TAG_TITLE_INFO_LIMITED-->"

#define		SKIN_KEYWORD_LINE_AVI_FPS		"<!--WIZD_INSERT_LINE_AVI_FPS-->"	// AVIのFPS

#define		SKIN_KEYWORD_LINE_AVI_DURATION	"<!--WIZD_INSERT_LINE_AVI_DURATION-->"	// AVIの再生時間

#define		SKIN_KEYWORD_LINE_AVI_VCODEC	"<!--WIZD_INSERT_LINE_AVI_VCODEC-->"	// AVIの動画コーデック

#define		SKIN_KEYWORD_LINE_AVI_ACODEC	"<!--WIZD_INSERT_LINE_AVI_ACODEC-->"	// AVIの音声コーデック

#define		SKIN_KEYWORD_LINE_AVI_HVCODEC	"<!--WIZD_INSERT_LINE_AVI_HVCODEC-->"	// AVIの動画コーデック(in the avi stream header)

#define		SKIN_KEYWORD_LINE_AVI_HACODEC	"<!--WIZD_INSERT_LINE_AVI_HACODEC-->"	// AVIの音声コーデック(in the avi stream header)

#define		SKIN_KEYWORD_LINE_AVI_IS_INTERLEAVED	"<!--WIZD_INSERT_LINE_AVI_IS_INTERLEAVED-->"	// AVIがインターリーブされているか

#define		SKIN_KEYWORD_LINE_READ			"<!--WIZD_INSERT_LINE_READ-->"


// 以下のキーワードで挟まれたエリアは、条件一致したときに削除される。

// ルートディレクトリの場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_ROOTDIR				"<!--WIZD_DELETE_IS_ROOTDIR-->"
#define		SKIN_KEYWORD_DEL_IS_ROOTDIR_E			"<!--/WIZD_DELETE_IS_ROOTDIR-->"

/* 後方互換 */
// 前ページが存在しない場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_NO_PAGE_PREV		"<!--WIZD_DELETE_IS_NO_PAGE_PREV-->"
#define		SKIN_KEYWORD_DEL_IS_NO_PAGE_PREV_E		"<!--/WIZD_DELETE_IS_NO_PAGE_PREV-->"

// 次ページが存在しない場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_NO_PAGE_NEXT		"<!--WIZD_DELETE_IS_NO_PAGE_NEXT-->"
#define		SKIN_KEYWORD_DEL_IS_NO_PAGE_NEXT_E		"<!--/WIZD_DELETE_IS_NO_PAGE_NEXT-->"

// 再生可能ファイルが存在しない場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_NO_STREAM_FILES		"<!--WIZD_DELETE_IS_NO_STREAM_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_NO_STREAM_FILES_E	"<!--/WIZD_DELETE_IS_NO_STREAM_FILES-->"

#define		SKIN_KEYWORD_DEL_IS_NO_MUSIC_FILES		"<!--WIZD_DELETE_IS_NO_MUSIC_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_NO_MUSIC_FILES_E	"<!--/WIZD_DELETE_IS_NO_MUSIC_FILES-->"

#define		SKIN_KEYWORD_DEL_IS_NO_PHOTO_FILES		"<!--WIZD_DELETE_IS_NO_PHOTO_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_NO_PHOTO_FILES_E	"<!--/WIZD_DELETE_IS_NO_PHOTO_FILES-->"

#define		SKIN_KEYWORD_DEL_CAN_DELETE		"<!--WIZD_CAN_DELETE-->"
#define		SKIN_KEYWORD_DEL_CAN_DELETE_E		"<!--/WIZD_CAN_DELETE-->"

/* 新規, DELETE (IF THERE) IS NO... と読む */
// 前ページが存在しない場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_NO_PAGE_PREV2		"<!--WIZD_IF_PAGE_PREV-->"
#define		SKIN_KEYWORD_DEL_IS_NO_PAGE_PREV2_E		"<!--/WIZD_IF_PAGE_PREV-->"

// 次ページが存在しない場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_NO_PAGE_NEXT2		"<!--WIZD_IF_PAGE_NEXT-->"
#define		SKIN_KEYWORD_DEL_IS_NO_PAGE_NEXT2_E		"<!--/WIZD_IF_PAGE_NEXT-->"

// 再生可能ファイルが存在しない場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_NO_STREAM_FILES2	"<!--WIZD_IF_STREAM_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_NO_STREAM_FILES2_E	"<!--/WIZD_IF_STREAM_FILES-->"

#define		SKIN_KEYWORD_DEL_IS_NO_MUSIC_FILES2		"<!--WIZD_IF_MUSIC_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_NO_MUSIC_FILES2_E	"<!--/WIZD_IF_MUSIC_FILES-->"

#define		SKIN_KEYWORD_DEL_IS_NO_PHOTO_FILES2		"<!--WIZD_IF_PHOTO_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_NO_PHOTO_FILES2_E	"<!--/WIZD_IF_PHOTO_FILES-->"

// 前ページが存在する場合削除 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_PAGE_PREV		"<!--WIZD_IF_NO_PAGE_PREV-->"
#define		SKIN_KEYWORD_DEL_IS_PAGE_PREV_E		"<!--/WIZD_IF_NO_PAGE_PREV-->"

// 次ページが存在する場合削除 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_PAGE_NEXT		"<!--WIZD_IF_NO_PAGE_NEXT-->"
#define		SKIN_KEYWORD_DEL_IS_PAGE_NEXT_E		"<!--/WIZD_IF_NO_PAGE_NEXT-->"

// 再生可能ファイルが存在する場合削除 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_STREAM_FILES	"<!--WIZD_IF_NO_STREAM_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_STREAM_FILES_E	"<!--/WIZD_IF_NO_STREAM_FILES-->"

#define		SKIN_KEYWORD_DEL_IS_MUSIC_FILES		"<!--WIZD_IF_NO_MUSIC_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_MUSIC_FILES_E	"<!--/WIZD_IF_NO_MUSIC_FILES-->"

#define		SKIN_KEYWORD_DEL_IS_PHOTO_FILES		"<!--WIZD_IF_NO_PHOTO_FILES-->"
#define		SKIN_KEYWORD_DEL_IS_PHOTO_FILES_E	"<!--/WIZD_IF_NO_PHOTO_FILES-->"

// MP3タグが存在しないとき (LINEのみ)
#define		SKIN_KEYWORD_DEL_IS_NO_MP3_TAGS			"<!--WIZD_DELETE_IS_NO_MP3_TAGS-->"
#define		SKIN_KEYWORD_DEL_IS_NO_MP3_TAGS_E		"<!--/WIZD_DELETE_IS_NO_MP3_TAGS-->"

// MP3タグが存在するとき (LINEのみ)
#define		SKIN_KEYWORD_DEL_IS_HAVE_MP3_TAGS		"<!--WIZD_DELETE_IS_HAVE_MP3_TAGS-->"
#define		SKIN_KEYWORD_DEL_IS_HAVE_MP3_TAGS_E		"<!--/WIZD_DELETE_IS_HAVE_MP3_TAGS-->"

// MP3タグが存在するとき (LINEのみ)
#define		SKIN_KEYWORD_DEL_IS_NOT_HAVE_AVI_TAGS		"<!--WIZD_DELETE_IS_NOT_HAVE_AVI_TAGS-->"
#define		SKIN_KEYWORD_DEL_IS_NOT_HAVE_AVI_TAGS_E		"<!--/WIZD_DELETE_IS_NOT_HAVE_AVI_TAGS-->"
#define		SKIN_KEYWORD_DEL_IS_HAVE_AVI_TAGS		"<!--WIZD_DELETE_IS_HAVE_AVI_TAGS-->"
#define		SKIN_KEYWORD_DEL_IS_HAVE_AVI_TAGS_E		"<!--/WIZD_DELETE_IS_HAVE_AVI_TAGS-->"


// 行が奇数/偶数のとき (LINEのみ)
#define		SKIN_KEYWORD_DEL_IF_LINE_IS_ODD			"<!--WIZD_IF_LINE_IS_EVEN-->"	// 行が奇数のとき削除
#define		SKIN_KEYWORD_DEL_IF_LINE_IS_ODD_E		"<!--/WIZD_IF_LINE_IS_EVEN-->"	// 行が奇数のとき削除

#define		SKIN_KEYWORD_DEL_IF_LINE_IS_EVEN		"<!--WIZD_IF_LINE_IS_ODD-->"	// 行が偶数のとき削除
#define		SKIN_KEYWORD_DEL_IF_LINE_IS_EVEN_E		"<!--/WIZD_IF_LINE_IS_ODD-->"	// 行が偶数のとき削除


// クライアントがPCのとき削除
#define		SKIN_KEYWORD_DEL_IF_CLIENT_IS_NOT_PC		"<!--WIZD_IF_CLIENT_IS_NOT_PC-->"
#define		SKIN_KEYWORD_DEL_IF_CLIENT_IS_NOT_PC_E		"<!--/WIZD_IF_CLIENT_IS_NOT_PC-->"

// クライアントがPCではないとき削除
#define		SKIN_KEYWORD_DEL_IF_CLIENT_IS_PC		"<!--WIZD_IF_CLIENT_IS_PC-->"
#define		SKIN_KEYWORD_DEL_IF_CLIENT_IS_PC_E		"<!--/WIZD_IF_CLIENT_IS_PC-->"

// Skin sections which depend on whether or not the client is using HD resolution
#define		SKIN_KEYWORD_DEL_IF_CLIENT_IS_HD		"<!--WIZD_IF_CLIENT_IS_HD-->"
#define		SKIN_KEYWORD_DEL_IF_CLIENT_IS_HD_E		"<!--/WIZD_IF_CLIENT_IS_HD-->"

#define		SKIN_KEYWORD_DEL_IF_CLIENT_IS_NOT_HD		"<!--WIZD_IF_CLIENT_IS_NOT_HD-->"
#define		SKIN_KEYWORD_DEL_IF_CLIENT_IS_NOT_HD_E		"<!--/WIZD_IF_CLIENT_IS_NOT_HD-->"

// focus が指定されているとき削除
#define		SKIN_KEYWORD_DEL_IF_FOCUS_IS_SPECIFIED		"<!--WIZD_IF_FOCUS_IS_NOT_SPECIFIED-->"
#define		SKIN_KEYWORD_DEL_IF_FOCUS_IS_SPECIFIED_E	"<!--/WIZD_IF_FOCUS_IS_NOT_SPECIFIED-->"

// focus が指定されていないとき削除
#define		SKIN_KEYWORD_DEL_IF_FOCUS_IS_NOT_SPECIFIED		"<!--WIZD_IF_FOCUS_IS_SPECIFIED-->"
#define		SKIN_KEYWORD_DEL_IF_FOCUS_IS_NOT_SPECIFIED_E	"<!--/WIZD_IF_FOCUS_IS_SPECIFIED-->"

// dvdopt
#define		SKIN_KEYWORD_DVD_OPTIONS	"<!--WIZD_INSERT_DVD_OPTIONS-->"


#define		SKIN_IMAGE_VIEWER_HTML 	"image_viewer.html"	// ImageViewerのスキン
#define		SKIN_JPEG_IMAGE_VIEWER_HTML 	"jpeg_image_viewer.html"	// ImageViewerのスキン
#define		SKIN_OPTION_MENU_HTML 	"option_menu.html"


#define		SKIN_KEYWORD_IMAGE_VIEWER_WIDTH		"<!--WIZD_INSERT_IMAGE_VIEWER_WIDTH-->"		// ImageViewerの表示横幅
#define		SKIN_KEYWORD_IMAGE_VIEWER_HEIGHT	"<!--WIZD_INSERT_IMAGE_VIEWER_HEIGHT-->"	// ImageViewerの表示高さ
#define		SKIN_KEYWORD_IMAGE_VIEWER_MODE		"<!--WIZD_INSERT_IMAGE_VIEWER_MODE-->"		// ImageViewerの現在のモード



// FITモードの時、以下の範囲を削除
#define		SKIN_KEYWORD_DEL_IS_FIT_MODE	"<!--WIZD_DELETE_IS_FIT_MODE-->"
#define		SKIN_KEYWORD_DEL_IS_FIT_MODE_E	"<!--/WIZD_DELETE_IS_FIT_MODE-->"


// FITモード以外の時、以下の範囲を削除
#define		SKIN_KEYWORD_DEL_IS_NO_FIT_MODE		"<!--WIZD_DELETE_IS_NO_FIT_MODE-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FIT_MODE_E	"<!--/WIZD_DELETE_IS_NO_FIT_MODE-->"

// 次ページが存在しない場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_CURRENT_PAGE		"<!--WIZD_DELETE_IS_CURRENT_PAGE-->"
#define		SKIN_KEYWORD_DEL_IS_CURRENT_PAGE_E		"<!--/WIZD_DELETE_IS_CURRENT_PAGE-->"

// 次ページが存在しない場合 (HEAD/TAILのみ)
#define		SKIN_KEYWORD_DEL_IS_NOT_CURRENT_PAGE		"<!--WIZD_DELETE_IS_NOT_CURRENT_PAGE-->"
#define		SKIN_KEYWORD_DEL_IS_NOT_CURRENT_PAGE_E		"<!--/WIZD_DELETE_IS_NOT_CURRENT_PAGE-->"

// setup the 10 favorites tests
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES1		"<!--WIZD_DELETE_IS_NO_FAVORITES1-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES1_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES1-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES2		"<!--WIZD_DELETE_IS_NO_FAVORITES2-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES2_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES2-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES3		"<!--WIZD_DELETE_IS_NO_FAVORITES3-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES3_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES3-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES4		"<!--WIZD_DELETE_IS_NO_FAVORITES4-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES4_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES4-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES5		"<!--WIZD_DELETE_IS_NO_FAVORITES5-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES5_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES5-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES6		"<!--WIZD_DELETE_IS_NO_FAVORITES6-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES6_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES6-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES7		"<!--WIZD_DELETE_IS_NO_FAVORITES7-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES7_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES7-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES8		"<!--WIZD_DELETE_IS_NO_FAVORITES8-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES8_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES8-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES9		"<!--WIZD_DELETE_IS_NO_FAVORITES9-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES9_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES9-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES10		"<!--WIZD_DELETE_IS_NO_FAVORITES10-->"
#define		SKIN_KEYWORD_DEL_IS_NO_FAVORITES10_E		"<!--/WIZD_DELETE_IS_NO_FAVORITES10-->"

// define 10 favorites variables
#define		SKIN_KEYWORD_FAVORITES1		    "<!--WIZD_FAVORITES1-->"
#define		SKIN_KEYWORD_FAVORITES2		    "<!--WIZD_FAVORITES2-->"
#define		SKIN_KEYWORD_FAVORITES3		    "<!--WIZD_FAVORITES3-->"
#define		SKIN_KEYWORD_FAVORITES4		    "<!--WIZD_FAVORITES4-->"
#define		SKIN_KEYWORD_FAVORITES5		    "<!--WIZD_FAVORITES5-->"
#define		SKIN_KEYWORD_FAVORITES6		    "<!--WIZD_FAVORITES6-->"
#define		SKIN_KEYWORD_FAVORITES7		    "<!--WIZD_FAVORITES7-->"
#define		SKIN_KEYWORD_FAVORITES8		    "<!--WIZD_FAVORITES8-->"
#define		SKIN_KEYWORD_FAVORITES9		    "<!--WIZD_FAVORITES9-->"
#define		SKIN_KEYWORD_FAVORITES10		    "<!--WIZD_FAVORITES10-->"

// for html movie info files
#define		SKIN_KEYWORD_DEL_IF_NO_HTML_LINK		"<!--WIZD_IF_NO_HTML_LINK-->"
#define		SKIN_KEYWORD_DEL_IF_NO_HTML_LINK_E	"<!--/WIZD_IF_NO_HTML_LINK-->"
#define		SKIN_KEYWORD_DEL_IF_HAVE_HTML_LINK		"<!--WIZD_IF_HAVE_HTML_LINK-->"
#define		SKIN_KEYWORD_DEL_IF_HAVE_HTML_LINK_E	"<!--/WIZD_IF_HAVE_HTML_LINK-->"

// for button images
#define		SKIN_KEYWORD_DEL_IF_NO_FILE_IMAGE		"<!--WIZD_IF_NO_FILE_IMAGE-->"
#define		SKIN_KEYWORD_DEL_IF_NO_FILE_IMAGE_E	"<!--/WIZD_IF_NO_FILE_IMAGE-->"
#define		SKIN_KEYWORD_DEL_IF_HAVE_FILE_IMAGE		"<!--WIZD_IF_HAVE_FILE_IMAGE-->"
#define		SKIN_KEYWORD_DEL_IF_HAVE_FILE_IMAGE_E	"<!--/WIZD_IF_HAVE_FILE_IMAGE-->"

//
#define		SKIN_KEYWORD_DEFAULT_TYPE		"<!--WIZD_DEFAULT_TYPE-->"
#define		SKIN_KEYWORD_ALIAS_SEARCH		"<!--WIZD_ALIAS_SEARCH-->"
#define		SKIN_KEYWORD_SEARCH_STR		    "<!--WIZD_SEARCH_STR-->"
#define		SKIN_KEYWORD_DEFAULT_SEARCH_ALIAS		    "<!--WIZD_SEARCH_ALIAS-->"

#endif
