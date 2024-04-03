// ==========================================================================
//code=EUC	tab=4
//
// wizd:	MediaWiz Server daemon.
//
// 		wizd_tools.c
//											$Revision: 1.27 $
//											$Date: 2007/02/23 00:34:11 $
//
//	すべて自己責任でおながいしまつ。
//  このソフトについてVertexLinkに問い合わせないでください。
// ==========================================================================
#include        <sys/types.h>

#include 		<ctype.h>
#include        <stdio.h>
#include		<stdlib.h>
#include 		<stdarg.h>
#include		<string.h>
#include		<limits.h>
#include 		<time.h>
#include 		<sys/stat.h>
#include        <errno.h>
#include 		<sys/types.h>
#include 		<fcntl.h>
#include 		<unistd.h>
# ifdef HAVE_W32API
#include <shlobj.h>
#include <objbase.h>
#include <ole2.h>
# endif

#include		"wizd.h"
#include		"wizd_tools.h"


static unsigned char		debug_log_filename[WIZD_FILENAME_MAX];	// デバッグログ出力ファイル名(フルパス)
static unsigned char		debug_log_initialize_flag  = (-1);	// デバッグログ初期化フラグ


# ifdef HAVE_W32API
// routine to find what a shortcut is pointing at
int
get_target(char *target_fname, char *retPath)
{
    HRESULT hres;
    IShellLink *shell_link;
    IPersistFile *persist_file;
    int result = 0;
	WCHAR wsz[MAX_PATH];

    hres = OleInitialize(NULL);
	if (hres != S_FALSE && hres != S_OK)
		return (-1);

    // Get a pointer to the IShellLink interface
    hres = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
			  &IID_IShellLink, (void **) &shell_link);

	if (!SUCCEEDED(hres)) {
		OleUninitialize();
		return (-1);
	}

    // Get a pointer to the IPersistFile interface
    hres = shell_link->lpVtbl->QueryInterface(shell_link, &IID_IPersistFile,
					    (void **) &persist_file);

	if (SUCCEEDED(hres)) {
		// Ensure that the string is Unicode
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR) target_fname, -1, wsz, MAX_PATH);

		// Load the shortcut
		hres = persist_file->lpVtbl->Load(persist_file, wsz, STGM_READ);

		if (SUCCEEDED(hres)) {
			// read stuff from the link object and print it to the screen
			shell_link->lpVtbl->GetPath(shell_link, retPath,
							 MAX_PATH, NULL, SLGP_RAWPATH);
		} else
			result = -1;

		// Release the pointer to the IPersistFile interface
		persist_file->lpVtbl->Release(persist_file);
	}

	OleUninitialize();
	return(result);
}
# endif

/********************************************************************************/
// sentence文字列内のkey文字列をrep文字列で置換する。
/********************************************************************************/
void replase_character(unsigned char *sentence, int sentence_buf_size, const unsigned char *key, const unsigned char *rep)
{
	int	malloc_size;
	unsigned char	*p, *buf;


	if ( strlen(key) == 0 )
		return;

	malloc_size = strlen(sentence) * 4;
	buf = malloc(malloc_size);
	if ( buf == NULL )
		return;

	p = strstr(sentence, key);

	while (p != NULL)
	{
		*p = '\0';
		strncpy(buf, p+strlen(key), malloc_size );
		strncat(sentence, rep, sentence_buf_size - strlen(sentence) );
		strncat(sentence, buf, sentence_buf_size - strlen(sentence) );
		p = strstr(p+strlen(rep), key);
	}

	free(buf);

	return;
}

/********************************************************************************/
// sentence文字列内の最初のkey文字列をrep文字列で置換する。
/********************************************************************************/
void replase_character_first(unsigned char *sentence, int sentence_buf_size, const unsigned char *key, const unsigned char *rep)
{
	int	malloc_size;
	unsigned char	*p, *buf;


	if ( strlen(key) == 0 )
		return;

	malloc_size = strlen(sentence) * 4;
	buf = malloc(malloc_size);
	if ( buf == NULL )
		return;

	p = strstr(sentence, key);

	if (p != NULL)
	{
		*p = '\0';
		strncpy(buf, p+strlen(key), malloc_size );
		strncat(sentence, rep, sentence_buf_size - strlen(sentence) );
		strncat(sentence, buf, sentence_buf_size - strlen(sentence) );
	}

	free(buf);

	return;
}

// **************************************************************************
// sentence 文字列内の、start_key文字列とend_key文字列に挟まれた部分を削除する
// **************************************************************************
void cut_enclose_words(unsigned char *sentence, int sentence_buf_size, unsigned char *start_key, unsigned char *end_key)
{
	int	malloc_size;
	unsigned char *start_p, *end_p, *buf;

	malloc_size = strlen(sentence) *2;
	buf = malloc(malloc_size);
	if ( buf == NULL )
		return;

	// start_keyを探す。
	start_p = strstr(sentence, start_key);

	while ( start_p != NULL )
	{
		// start_key を削除
		*start_p = '\0';

		// start_p の後ろをbufにコピー
		strncpy(buf, start_p+strlen(start_key), malloc_size );

		// end_keyを探す
		end_p = strstr(buf, end_key);

		if ( end_p == NULL )
		{
			// end_keyが無ければ、

			// 削除したstart_key を復活
			strncat( sentence, start_key, sentence_buf_size - strlen(sentence) );

			//後ろ全部くっつける。
			strncat( sentence, buf, sentence_buf_size - strlen(sentence) );
			break;
		}

		// end_key 見つかった。

		// end_keyの後ろをsentenceにくっつける。
		strncat( sentence, end_p+strlen(end_key), sentence_buf_size - strlen(sentence) );

		// 再度start_keyを探す。
		start_p = strstr(sentence, start_key);
	}


	free(buf);
	return;
}


//***************************************************************************
// sentence文字列より、cut_charから後ろを削除
//	見つからなければ何もしない。
//***************************************************************************
void 	cut_after_character(unsigned char *sentence, unsigned char cut_char)
{
	unsigned char 	*symbol_p;

	// 削除対象キャラクターがあった場合、それから後ろを削除。
	symbol_p = strchr(sentence, cut_char);

	if (symbol_p != NULL)
	{
		*symbol_p = '\0';
	}

	return;
}



//***************************************************************************
// sentence文字列の、cut_charが最初に出てきた所から前を削除
// もし、cut_charがsentence文字列に入っていなかった場合、文字列全部削除
//***************************************************************************
void 	cut_before_character(unsigned char *sentence, unsigned char cut_char)
{

	unsigned char 	*symbol_p;
	unsigned char	*malloc_p;
	int				sentence_len;


	if (sentence == NULL)
		return;

	sentence_len = strlen(sentence);


	// 削除対象キャラクターが最初に出てくる所を探す。
	symbol_p = strchr(sentence, cut_char);
	if (symbol_p == NULL)
	{
		// 発見できなかった場合、文字列全部削除。
		strncpy(sentence, "", sentence_len);
		return;
	}

	symbol_p++;

	// テンポラリエリアmalloc.
	malloc_p = malloc(sentence_len + 10);
	if (malloc_p == NULL)
		return;

	// 削除対象キャラクターの後ろから最後までの文字列をコピー
	strncpy(malloc_p, symbol_p, sentence_len + 10);

	// sentence書き換え
	strncpy(sentence, malloc_p, sentence_len);

	free(malloc_p);

	return;
}


//************************************************************************
// sentence文字列の、cut_charが最後に出てきた所から前を削除
// もし、cut_charがsentence文字列に入っていなかった場合、なにもしない。
//************************************************************************
void 	cut_before_last_character(unsigned char *sentence, unsigned char cut_char)
{

	unsigned char 	*symbol_p;
	unsigned char	*malloc_p;
	int				sentence_len;

	if (sentence == NULL)
		return;

	sentence_len = strlen(sentence);

	// 削除対象キャラクターが最後に出てくる所を探す。
	symbol_p = strrchr(sentence, cut_char);
	if (symbol_p == NULL)
	{
		// 発見できなかった場合、なにもしない。
		return;
	}

	symbol_p++;

	// テンポラリエリアmalloc.
	malloc_p = malloc(sentence_len + 10);
	if (malloc_p == NULL)
		return;

	// 削除対象キャラクターの後ろから最後までの文字列をコピー
	strncpy(malloc_p, symbol_p, sentence_len + 10);

	// sentence書き換え
	strncpy(sentence, malloc_p, sentence_len);

	free(malloc_p);

	return;
}

//************************************************************************
// sentence文字列の、cut_charが最後に出てきた所から後ろをCUT
// もし、cut_charがsentence文字列に入っていなかった場合、文字列全部削除。
//************************************************************************
void 	cut_after_last_character(unsigned char *sentence, unsigned char cut_char)
{

	unsigned char 	*symbol_p;
	int				sentence_len;

	if (sentence == NULL)
		return;

	sentence_len = strlen(sentence);

	// 削除対象キャラクターが最後に出てくる所を探す。
	symbol_p = strrchr(sentence, cut_char);
	if (symbol_p == NULL)
	{
		// 発見できなかった場合、文字列全部削除。
		strncpy(sentence, "", sentence_len);
		return;
	}

	*symbol_p = '\0';

	return;
}


//******************************************************************
// sentenceの、後ろ n byteを残して削除。
//******************************************************************
void 	cat_before_n_length(unsigned char *sentence,  unsigned int n)
{
	unsigned char	*malloc_p;
	unsigned char	*work_p;

	int				sentence_len;

	if (sentence == NULL)
		return;

	sentence_len = strlen(sentence);

	// sentence が、nよりも同じか短いならばreturn
	if ( sentence_len <= n )
		return;


	// テンポラリエリアmalloc.
	malloc_p = malloc(sentence_len + 10);
	if (malloc_p == NULL)
		return;

	work_p = sentence;
	work_p += sentence_len;
	work_p -= n;
	strncpy(malloc_p, work_p, sentence_len + 10);
	strncpy( sentence, malloc_p, sentence_len);

	free(malloc_p);
	return;
}

//******************************************************************
// sentenceの、後ろ n byteを削除
//  全長がn byteに満たなかったら、文字列全部削除
//******************************************************************
void 	cat_after_n_length(unsigned char *sentence,  unsigned int n)
{
	unsigned char	*work_p;
	int				sentence_len;

	if (sentence == NULL)
		return;

	sentence_len = strlen(sentence);

	// sentence が、nよりも同じか短いならば、全部削除してreturn;
	if ( sentence_len <= n )
	{
		strncpy(sentence, "", sentence_len);
		return;
	}


	// 後ろ n byteを削除
	work_p = sentence;
	work_p += sentence_len;
	work_p -= n;
	*work_p = '\0';

	return;
}



//******************************************************************
// sentence文字列の、cut_charを抜く。
//******************************************************************
void 	cut_character(unsigned char *sentence, unsigned char cut_char)
{

	unsigned char 	*symbol_p;
	unsigned char	*malloc_p;
	unsigned char	*work_p;
	int				sentence_len;


	if (sentence == NULL)
		return;

	sentence_len = strlen(sentence);

	// テンポラリエリアmalloc.
	malloc_p = malloc(sentence_len + 10);
	if (malloc_p == NULL)
		return;

	symbol_p = sentence;
	work_p = malloc_p;


	// 処理ループ。
	while (*symbol_p != '\0')
	{
		// 削除対象のキャラクターがいたら、それを飛ばす。
		if (*symbol_p == cut_char)
		{
			symbol_p++;
		}
		else	// 削除対象キャラクター意外だったら、コピー。
		{
			*work_p = *symbol_p;
			work_p++;
			symbol_p++;
		}
	}

	// '\0' をコピー。
	*work_p = *symbol_p;

	// sentence書き換え
	strncpy(sentence, malloc_p, sentence_len);

	free(malloc_p);

	return;
}

//******************************************************************
// sentence文字列の、頭にcut_charがいたら、抜く。
//******************************************************************
void 	cut_first_character(unsigned char *sentence, unsigned char cut_char)
{

	unsigned char	*malloc_p;
	unsigned char	*work_p;
	int				sentence_len;

	if (sentence == NULL)
		return;

	sentence_len = strlen(sentence);

	// テンポラリエリアmalloc.
	malloc_p = malloc(sentence_len + 10);
	if (malloc_p == NULL)
		return;

	strncpy(malloc_p, sentence, sentence_len + 10);

	work_p = malloc_p;



	// 削除対象キャラクターがあるかぎり進める。
	while ((*work_p == cut_char) && (*work_p != '\0'))
	{
		work_p++;
	}

	// sentence書き換え
	strncpy(sentence, work_p, sentence_len);

	free(malloc_p);

	return;
}

// ***************************************************************************
// sentence文字列の行末に、cut_charがあったとき、削除
// ***************************************************************************
void 	cut_character_at_linetail(char *sentence, char cut_char)
{
	char	*source_p;
	int		length, i;


	if (sentence == NULL)
		return;

	length = strlen(sentence);	// 文字列長Get

	source_p = sentence;
	source_p += length;		// ワークポインタを文字列の最後にセット。


	for (i=0; i<length; i++)	// 文字列の数だけ繰り返し。
	{
		source_p--;			// 一文字ずつ前へ。

		if (*source_p == cut_char)	// 削除キャラ ヒットした場合削除
		{
			*source_p = '\0';
		}
		else						// 違うキャラが出てきたところで終了。
		{
			break;
		}
	}

	return;
}


void 	cut_whitespace_at_linetail(char *sentence)
{
	char	*source_p;
	int		length, i;


	if (sentence == NULL)
		return;

	length = strlen(sentence);	// 文字列長Get

	source_p = sentence;
	source_p += length;		// ワークポインタを文字列の最後にセット。


	for (i=0; i<length; i++)	// 文字列の数だけ繰り返し。
	{
		source_p--;			// 一文字ずつ前へ。

		if (isspace(*source_p))	// 削除キャラ ヒットした場合削除
		{
			*source_p = '\0';
		}
		else						// 違うキャラが出てきたところで終了。
		{
			break;
		}
	}

	return;
}

/********************************************************************************/
// sentence文字列内のunique_charが連続しているところを、unique_char1文字だけにする。
/********************************************************************************/
void duplex_character_to_unique(unsigned char *sentence, unsigned char unique_char)
{
	unsigned char	*source_p, *work_p;
	unsigned char 	*work_malloc_p;
	unsigned char	unique_char_count = 0;
	int				org_sentence_len;


	if (sentence == NULL)
		return;

	// オリジナル文字列長を保存。
	org_sentence_len = strlen(sentence);

	// ワークバッファ確保。
	work_malloc_p = malloc( org_sentence_len+10 );
	if ( work_malloc_p == NULL )
		return;

	source_p = sentence;
	work_p = work_malloc_p;

	// sensense文字列から、unique_char以外をワークへコピー。
	while (*source_p != '\0')
	{
		if (*source_p == unique_char)	// unique_char発見
		{
			if (unique_char_count == 0)	// 最初の一つならコピー。それ以外ならスキップ。
			{
				*work_p = *source_p;
				work_p++;
			}
			unique_char_count++;
		}
		else	// unique_char 以外ならコピー。
		{
			unique_char_count = 0;
			*work_p = *source_p;
			work_p++;
		}
		source_p++;
	}

	*work_p = '\0';

	// ワークから、sentenceへ、結果を書き戻す。
	strncpy(sentence, work_malloc_p, org_sentence_len );

	free( work_malloc_p );	// Mem Free.

	return;
}




//*********************************************************
// sentence文字列より、最初に出て来たcut_charの前後を分割。
//
//	sentence	(IN) 分割対象の文字列を与える。
//	cut_char	(IN) 分割対象の文字を入れる。
//	split1		(OUT)カットされた前の部分が入る。sentenceと同等のサイズが望ましい。
//	split2		(OUT)カットされた後ろの部分が入る。sentenceと同等のサイズが望ましい。
//
//
// return
//		0: 			正常終了。
//		それ以外：	エラー。分割失敗などなど。
//*********************************************************
int sentence_split(unsigned char *sentence, unsigned char cut_char, unsigned char *split1, unsigned char *split2)
{
	unsigned char 	*p;
	unsigned char	*malloc_p;
	int				sentence_len;

	// エラーチェック。
	if ((sentence == NULL)||(split1 == NULL)||(split2 == NULL))
	{
		return 1;	// 引数にNULLまじり。
	}

	// sentence の長さをGet.
	sentence_len = strlen(sentence);


	// ワーク領域malloc.
	malloc_p = malloc(sentence_len + 10);

	if (malloc_p == NULL)
	{
		// malloc 失敗。エラー。
		return 1;
	}

	// sentence文字列をワークにコピー。
	strncpy(malloc_p, sentence, sentence_len + 10);

	// sentence 内に、cut_char が有るかチェック。無ければエラー。
	p = strchr(malloc_p, cut_char);
	if (p == NULL)
	{
		free(malloc_p);
		return 1;	// 分割文字発見できず。
	}

	// cut_charより、後ろをカット。
	*p = '\0';

	// 前半部分をコピー。
	strncpy(split1, malloc_p, sentence_len);

	// 後半部分をコピー。
	p++;
	strncpy(split2, p, sentence_len);


	free(malloc_p);

	return 0; // 正常終了。
}

//******************************************************************
// filenameから、拡張子を取り出す('.'も消す）
// '.'が存在しなかった場合、拡張子が長すぎた場合は、""が入る。
//******************************************************************
void filename_to_extension(unsigned char *filename, unsigned char *extension_buf, int extension_buf_size)
{
	unsigned char	*p;


	// 拡張子の存在チェック。
	p = strrchr(filename, '.' );
	if (( p == NULL ) || ( strlen(p) > extension_buf_size ))
	{
		strncpy(extension_buf, "", extension_buf_size );
		return;
	}


	// 拡張子を切り出し。
	p++;
	strncpy( extension_buf, p, extension_buf_size );

	return;
}


// **************************************************************************
// text_buf から、CR/LF か'\0'が現れるまでを切り出して、line_bufにcopy。
// (CR/LFはcopyされない)
// 次の行の頭のポインタをreturn。
// Errorか'\0'が現れたらNULLが戻る。
// **************************************************************************
unsigned char *buffer_distill_line(unsigned char *text_buf_p, unsigned char *line_buf_p, unsigned int line_buf_size )
{
	unsigned char	*p;
	int			counter = 0;

	p = text_buf_p;

	// ------------------
	// CR/LF '\0'を探す
	// ------------------
	while ( 1 )
	{
		if ( *p == '\r' ) // CR
		{
			p++;
			continue;
		}

		if ( *p == '\n' )	// LF
		{
			p++;
			break;
		}

		if ( *p == '\0' )
		{
			break;
		}

		p++;
		counter++;
	}

	// --------------------------------------------------
	// 数えた文字数だけ、line_buf_p に文字列をコピー
	// --------------------------------------------------
	memset(line_buf_p , '\0', line_buf_size );
	if ( counter >= line_buf_size )
	{
		counter = (line_buf_size -1);
	}
	strncpy(line_buf_p, text_buf_p, counter);

	if ( *p == '\0' )
		return NULL;		// バッファの最後
	else
		return p;			// バッファの途中

}



// **************************************************************************
//  URIエンコードを行います.
//  機能 : URIデコードを行う
//  書式 : int uri_encode
//  (char* dst,size_t dst_len,const char* src,int src_len);
//  引数 : dst 変換した文字の書き出し先.
// 		   dst_len 変換した文字の書き出し先の最大長.
// 		   src 変換元の文字.
// 		   src_len 変換元の文字の長さ.
//  返値 : エンコードした文字の数(そのままも含む)
// **************************************************************************
int uri_encode(unsigned char *dst,  unsigned int dst_len, const unsigned char *src, int src_len)
{
	int idx_src;
	int idx_dst;
	int cnt;


	// 引数チェック
	if((dst == NULL) || (dst_len < 1))
	{
		return 0;
	}

	if((src == NULL) || (src_len < 1)) 
	{
		*dst = 0;
		return 0;
	}
	cnt = 0;
	for (idx_src = idx_dst = 0 ; (idx_src < src_len) && (idx_dst < dst_len) && (src[idx_src] != '\0'); idx_src++)
	{
		/* エンコードしない文字集合 */
		if ( strchr("-./_", src[idx_src]) != NULL )
		{
			dst[idx_dst] = src[idx_src];
			idx_dst += 1;
		}
		/* アルファベットと数字はエンコードせずそのまま */
		else if ( isalnum( src[idx_src] ) )
		{
			dst[idx_dst] = src[idx_src];
			idx_dst += 1;
		}
		/* それ以外はすべてエンコード */
		else
		{
			if ((idx_dst + 3) > dst_len)
				break;

			idx_dst += sprintf(&dst[idx_dst],"%%%02X",(unsigned char)(src[idx_src]));
		}
		cnt++;

		if ((idx_dst + 1) < dst_len)
		{
			dst[idx_dst] = '\0';
		}
	}

	return cnt;
}


// **************************************************************************
// URIデコードを行います.
//  機能 : URIデコードを行う
//  引数 : dst 変換した文字の書き出し先.
//		  dst_len 変換した文字の書き出し先の最大長.
//		  src 変換元の文字.
//		  src_len 変換元の文字の長さ.
// 返値 : デコードした文字の数(そのままも含む)
// **************************************************************************
int uri_decode(unsigned char *dst, unsigned int dst_len, const unsigned char *src, unsigned int src_len)
{

	int 		idx_src;
	int 		idx_dst;
	int 		cnt;
	char 		work[3];
	const char	*ptr_stop;
	char		*strtol_end_ptr;
	int 		code;


	// 引数チェック
	if ((dst == NULL) || (dst_len < 1) || (src == NULL) || (src_len < 1))
	{
		return 0;
	}


	cnt = 0;
	ptr_stop = src;

	// =================
	// メインループ
	// =================
	for (idx_src = idx_dst = 0; (idx_src < src_len) && (idx_dst < dst_len) && (src[idx_src] != '\0'); idx_dst++ , cnt++)
	{
		if (src[idx_src] == '%')
		{
			if (idx_src + 2 > src_len)
			{
				break;
			}

			work[0] = src[idx_src+1];
			work[1] = src[idx_src+2];
			work[2] = '\0';

			code = strtol(work, &strtol_end_ptr, 16);
			ptr_stop = &src[idx_src + (strtol_end_ptr - work) + 1];

			if (code == LONG_MIN || code == LONG_MAX)
			{
				break;
			}

			if (strtol_end_ptr != NULL)
			{
				if (*strtol_end_ptr != '\0')
				{
					break;
				}
			}
			dst[idx_dst] = code;
			idx_src += 3;
		}
		else if ( src[idx_src] == '+' )
		{
			dst[idx_dst] = ' ';
			idx_src += 1;
			ptr_stop++;
		}
		else
		{
			dst[idx_dst] = src[idx_src];
			idx_src += 1;
			ptr_stop++;
		}

		if (idx_dst + 1 < dst_len)
		{
			dst[idx_dst + 1] = '\0';
		}
	}

	return cnt;
}

/********************************************************************************/
// "YYYY/MM/DD HH:MM:SS" 形式の現在の日時の文字列を生成する。
/********************************************************************************/
void make_datetime_string(unsigned char *sentence)
{
	time_t				now;
	struct tm			*tm_p;

	// 現在時刻Get.
	time(&now);
	tm_p = localtime(&now);

	sprintf(sentence, 	"%04d/%02d/%02d %02d:%02d:%02d",
						tm_p->tm_year+1900	,	// 年
						tm_p->tm_mon+1	,		// 月
						tm_p->tm_mday	,		// 日
						tm_p->tm_hour	,		// 時刻
						tm_p->tm_min	,		// 分
						tm_p->tm_sec		);	// 秒

	return;
}


/********************************************************************************/
// time_t から、"YYYY/MM/DD HH:MM" 形式の文字列を生成する。
/********************************************************************************/
void conv_time_to_string(unsigned char *sentence, time_t conv_time)
{
	struct tm			*tm_p;

	if (conv_time == 0) {
		sprintf(sentence, "--/--/-- --:--");
		return ;
	}
	tm_p = localtime(&conv_time);
	sprintf(sentence, 	"%04d/%02d/%02d %02d:%02d",
						tm_p->tm_year+1900	,	// 年
						tm_p->tm_mon+1	,		// 月
						tm_p->tm_mday	,		// 日
						tm_p->tm_hour	,		// 時刻
						tm_p->tm_min		);	// 分

	return;
}




/********************************************************************************/
// time_t から、"YYYY/MM/DD" 形式の文字列を生成する。
/********************************************************************************/
void conv_time_to_date_string(unsigned char *sentence, time_t conv_time)
{
	struct tm			*tm_p;

	if (conv_time == 0) {
		sprintf(sentence, "--/--/--");
		return ;
	}
	tm_p = localtime(&conv_time);
	sprintf(sentence, 	"%04d/%02d/%02d",
						tm_p->tm_year+1900	,	// 年
						tm_p->tm_mon+1	,		// 月
						tm_p->tm_mday		);	// 日

	return;
}




/********************************************************************************/
// time_t から、"HH:MM" 形式の文字列を生成する。
/********************************************************************************/
void conv_time_to_time_string(unsigned char *sentence, time_t conv_time)
{
	struct tm			*tm_p;

	if (conv_time == 0) {
		sprintf(sentence, "--:--");
		return ;
	}
	tm_p = localtime(&conv_time);
	sprintf(sentence, 	"%02d:%02d",
						tm_p->tm_hour	,		// 時刻
						tm_p->tm_min		);	// 分

	return;
}


void conv_duration_to_string(unsigned char *sentence, dvd_duration *dvdtime)
{
	if (dvdtime == NULL) {
		sprintf(sentence, "--:--:--");
		return ;
	}
	if (dvdtime->hour == -1) {
		sentence[0]=0;
		return;
	}
	sprintf(sentence, "%d:%02d:%02d",
			dvdtime->hour,
			dvdtime->minute,
			dvdtime->second);

	return;
}


/********************************************************************************/
// 100000000 → "100.00 MB" への変換を行う。
//
//	K,M,G に対応。
/********************************************************************************/
void conv_num_to_unit_string(unsigned char *sentence, u_int64_t file_size)
{
	u_int64_t	real_size;
	u_int64_t	little_size;


	if ((int) file_size == -1) {
		sentence[0] = 0;
		return;
	} else if ( file_size < 1024 )
	{
		sprintf(sentence, "%lld B", file_size );
	}
	else if ( file_size < (1024 * 1024) )
	{
		real_size 	= file_size / 1024;
		little_size = (file_size * 100 / 1024) % 100;

		sprintf(sentence, "%lld.%02lld KB", real_size, little_size);
	}
	else if ( file_size < (1024 * 1024 * 1024) )
	{
		real_size 	= file_size / ( 1024*1024 );
		little_size = (file_size * 100 / (1024*1024)) % 100;

		sprintf(sentence, "%lld.%02lld MB", real_size, little_size);
	}
	else
	{

		real_size 	= file_size / ( 1024*1024*1024 );
		little_size = (file_size * 100 / (1024*1024*1024)) % 100;

		sprintf(sentence, "%lld.%02lld GB", real_size, little_size);
	}

	return;
}








//*******************************************************************
// デバッグ出力初期化(ファイル名セット)関数
// この関数を最初に呼ぶまでは、デバッグログは一切出力されない。
//*******************************************************************
void debug_log_initialize(const unsigned char *set_debug_log_filename)
{
	// Accept NULL or zero-length debug log filename, and output to stderr in that case
	if (set_debug_log_filename == NULL) {
		memset(debug_log_filename, 0, sizeof(debug_log_filename));
	} else {
		// デバッグログファイル名をセット。
		strncpy(debug_log_filename, set_debug_log_filename,	sizeof(debug_log_filename) );
	}
	// デバッグログ 初期化完了フラグを0に。
	debug_log_initialize_flag = 0;

	return;
}



//*************************************************
// デバッグ出力用関数。
// printf() と同じフォーマットにて使用する。
//*************************************************
void debug_log_output(char *fmt, ...)
{
	FILE	*fp;
	unsigned char	buf[1024*5+1];
	unsigned char	work_buf[1024*4+1];
	unsigned char	date_and_time[32];
	unsigned char	replase_date_and_time[48];
	va_list 	arg;
	int		len;

	// =========================================
	// デバッグログ 初期化フラグをチェック
	// =========================================
	if (debug_log_initialize_flag != 0 )
		return;


	// =========================================
	// Debug出力文字列生成。
	// 行頭に、date_and_time を挿入しておく
	// =========================================
	memset(buf, '\0', sizeof(buf));
	memset(work_buf, '\0', sizeof(work_buf));

	// 引数で与えられた文字列を展開。
	va_start(arg, fmt);
	vsnprintf(work_buf, sizeof(work_buf), fmt, arg);
	va_end(arg);

	// work_bufの一番最後に'\n'がついていたら削除。
	len = strlen(work_buf);
	if (work_buf[len-1] == '\n')
	{
		work_buf[len-1] = '\0';
	}

	// 挿入用文字列生成( "\ndate_and_time" になる)
	make_datetime_string(date_and_time);
	snprintf(replase_date_and_time, sizeof(replase_date_and_time), "\n%s[%d] ", date_and_time, getpid() );

	// 出力文字列生成開始。
	snprintf(buf, sizeof(buf), "%s[%d] %s", date_and_time, getpid(), work_buf);
	replase_character(buf, sizeof(buf), "\n", replase_date_and_time);	// \nの前にdate_and_timeを挿入

	// 一番最後に'\n'をつける。
	strncat(buf, "\n", sizeof(buf)-strlen(buf) );


	// =====================
	// ログファイル出力
	// =====================

	// ファイルオープン（追記モード)
	if(debug_log_filename[0] == 0) {
		fwrite(buf, 1, strlen(buf), stdout);
	} else {
		fp = fopen(debug_log_filename, "a");
		if ( fp == NULL )
			return;
		// 出力
		fwrite(buf, 1, strlen(buf), fp );	// メッセージ実体を出力

		// ファイルクローズ
		fclose( fp );
	}

	return;
}



// **************************************************************************
// 拡張子変更処理。追加用。
//	extension_convert_listに従い、org → rename への変換を行う。
//
// 例) "hogehoge.m2p" → "hogehoge.m2p.mpg"
// **************************************************************************
void extension_add_rename(unsigned char *rename_filename_p, size_t rename_filename_size)
{
	int i;
	unsigned char	ext[WIZD_FILENAME_MAX];

	if ( rename_filename_p == NULL )
		return;

	filename_to_extension(rename_filename_p, ext, sizeof(ext));

	if (strlen(ext) <= 0) return;

	for ( i=0; extension_convert_list[i].org_extension != NULL; i++ )
	{
		debug_log_output("org='%s', rename='%s'"
			, extension_convert_list[i].org_extension
			, extension_convert_list[i].rename_extension);

		// 拡張子一致？
		if ( strcasecmp(ext, extension_convert_list[i].org_extension) == 0 )
		{
			debug_log_output(" HIT!!!" );
			// 拡張子を「追加」
			strncat(rename_filename_p, "."
				, rename_filename_size - strlen(rename_filename_p));
			strncat(rename_filename_p
				, extension_convert_list[i].rename_extension
				, rename_filename_size - strlen(rename_filename_p));

			debug_log_output("rename_filename_p='%s'", rename_filename_p);
			break;
		}
	}


	return;
}


// **************************************************************************
// 拡張子変更処理。削除用。
//	extension_convert_listに従い、rename → org への変換を行う。
//
// 例) "hogehoge.m2p.mpg" → "hogehoge.m2p"
// **************************************************************************
void extension_del_rename(unsigned char *rename_filename_p, size_t rename_filename_size)
{
	int i;
	unsigned char	renamed_ext[WIZD_FILENAME_MAX];
	unsigned char	ext[WIZD_FILENAME_MAX];

	if ( rename_filename_p == NULL )
		return;

	for ( i=0; extension_convert_list[i].org_extension != NULL; i++ )
	{
		debug_log_output("org='%s', rename='%s'"
			, extension_convert_list[i].org_extension
			, extension_convert_list[i].rename_extension);

		snprintf(renamed_ext, sizeof(renamed_ext), ".%s.%s"
			, extension_convert_list[i].org_extension
			, extension_convert_list[i].rename_extension);

		// 比較する拡張子と同じ長さにそろえる。
		strncpy(ext, rename_filename_p, sizeof(ext));
		cat_before_n_length(ext, strlen(renamed_ext));

		// 拡張子一致？
		if ( strcasecmp(ext, renamed_ext) == 0 )
		{
			debug_log_output(" HIT!!!" );
			// 拡張子を「削除」
			cat_after_n_length(rename_filename_p
				, strlen(extension_convert_list[i].rename_extension) + 1);

			debug_log_output("rename_filename_p='%s'", rename_filename_p);
			break;
		}
	}


	return;
}

// **************************************************************************
// * Remove underscore(_), dash(-), tab characters
// * and convert sequences of UPPERCASE into Uppercase
// **************************************************************************
void clean_buffer_text(char *buffer)
{
	int last_was_upper=0;
	while(*buffer) {
		if((*buffer == '\t') || (*buffer == '_') || (*buffer == '-'))
			*buffer = ' ';
		// Change sequences of UPPERCASE into Uppercase
		if(isupper(*buffer)) {
			if(last_was_upper)
				*buffer = tolower(*buffer);
			else
				last_was_upper = 1;
		} else {
			last_was_upper = 0;
		}
		buffer++;
	}
}


// **************************************************************************
// * EUC文字列中の半角文字を全角にする。
// **************************************************************************
void han2euczen(unsigned char *src, unsigned char *dist, int dist_size)
{

	unsigned char	*dist_p, *src_p;

	dist_p 	= dist;
	src_p	= src;

	while ( *src_p != '\0')
	{
		switch( (int)*src_p )
		{
			case ' ':	strncpy(dist_p, "  ", dist_size);	dist_p+=2;	break;
			case '!':	strncpy(dist_p, "！", dist_size);	dist_p+=2;	break;
			case '"':	strncpy(dist_p, "”", dist_size);	dist_p+=2;	break;
			case '#':	strncpy(dist_p, "＃", dist_size);	dist_p+=2;	break;
			case '$':	strncpy(dist_p, "＄", dist_size);	dist_p+=2;	break;
			case '%':	strncpy(dist_p, "％", dist_size);	dist_p+=2;	break;
			case '&':	strncpy(dist_p, "＆", dist_size);	dist_p+=2;	break;
			case '\'':	strncpy(dist_p, "’", dist_size);	dist_p+=2;	break;
			case '(':	strncpy(dist_p, "（", dist_size);	dist_p+=2;	break;
			case ')':	strncpy(dist_p, "）", dist_size);	dist_p+=2;	break;
			case '*':	strncpy(dist_p, "＊", dist_size);	dist_p+=2;	break;
			case '+':	strncpy(dist_p, "＋", dist_size);	dist_p+=2;	break;
			case ',':	strncpy(dist_p, "，", dist_size);	dist_p+=2;	break;
			case '-':	strncpy(dist_p, "ー", dist_size);	dist_p+=2;	break;
			case '.':	strncpy(dist_p, "．", dist_size);	dist_p+=2;	break;
			case '/':	strncpy(dist_p, "／", dist_size);	dist_p+=2;	break;
			case '0':	strncpy(dist_p, "０", dist_size);	dist_p+=2;	break;
			case '1':	strncpy(dist_p, "１", dist_size);	dist_p+=2;	break;
			case '2':	strncpy(dist_p, "２", dist_size);	dist_p+=2;	break;
			case '3':	strncpy(dist_p, "３", dist_size);	dist_p+=2;	break;
			case '4':	strncpy(dist_p, "４", dist_size);	dist_p+=2;	break;
			case '5':	strncpy(dist_p, "５", dist_size);	dist_p+=2;	break;
			case '6':	strncpy(dist_p, "６", dist_size);	dist_p+=2;	break;
			case '7':	strncpy(dist_p, "７", dist_size);	dist_p+=2;	break;
			case '8':	strncpy(dist_p, "８", dist_size);	dist_p+=2;	break;
			case '9':	strncpy(dist_p, "９", dist_size);	dist_p+=2;	break;
			case ':':	strncpy(dist_p, "：", dist_size);	dist_p+=2;	break;
			case '<':	strncpy(dist_p, "＜", dist_size);	dist_p+=2;	break;
			case '=':	strncpy(dist_p, "＝", dist_size);	dist_p+=2;	break;
			case '>':	strncpy(dist_p, "＞", dist_size);	dist_p+=2;	break;
			case '?':	strncpy(dist_p, "？", dist_size);	dist_p+=2;	break;
			case '@':	strncpy(dist_p, "＠", dist_size);	dist_p+=2;	break;
			case 'A':	strncpy(dist_p, "Ａ", dist_size);	dist_p+=2;	break;
			case 'B':	strncpy(dist_p, "Ｂ", dist_size);	dist_p+=2;	break;
			case 'C':	strncpy(dist_p, "Ｃ", dist_size);	dist_p+=2;	break;
			case 'D':	strncpy(dist_p, "Ｄ", dist_size);	dist_p+=2;	break;
			case 'E':	strncpy(dist_p, "Ｅ", dist_size);	dist_p+=2;	break;
			case 'F':	strncpy(dist_p, "Ｆ", dist_size);	dist_p+=2;	break;
			case 'G':	strncpy(dist_p, "Ｇ", dist_size);	dist_p+=2;	break;
			case 'H':	strncpy(dist_p, "Ｈ", dist_size);	dist_p+=2;	break;
			case 'I':	strncpy(dist_p, "Ｉ", dist_size);	dist_p+=2;	break;
			case 'J':	strncpy(dist_p, "Ｊ", dist_size);	dist_p+=2;	break;
			case 'K':	strncpy(dist_p, "Ｋ", dist_size);	dist_p+=2;	break;
			case 'L':	strncpy(dist_p, "Ｌ", dist_size);	dist_p+=2;	break;
			case 'M':	strncpy(dist_p, "Ｍ", dist_size);	dist_p+=2;	break;
			case 'N':	strncpy(dist_p, "Ｎ", dist_size);	dist_p+=2;	break;
			case 'O':	strncpy(dist_p, "Ｏ", dist_size);	dist_p+=2;	break;
			case 'P':	strncpy(dist_p, "Ｐ", dist_size);	dist_p+=2;	break;
			case 'Q':	strncpy(dist_p, "Ｑ", dist_size);	dist_p+=2;	break;
			case 'R':	strncpy(dist_p, "Ｒ", dist_size);	dist_p+=2;	break;
			case 'S':	strncpy(dist_p, "Ｓ", dist_size);	dist_p+=2;	break;
			case 'T':	strncpy(dist_p, "Ｔ", dist_size);	dist_p+=2;	break;
			case 'U':	strncpy(dist_p, "Ｕ", dist_size);	dist_p+=2;	break;
			case 'V':	strncpy(dist_p, "Ｖ", dist_size);	dist_p+=2;	break;
			case 'W':	strncpy(dist_p, "Ｗ", dist_size);	dist_p+=2;	break;
			case 'X':	strncpy(dist_p, "Ｘ", dist_size);	dist_p+=2;	break;
			case 'Y':	strncpy(dist_p, "Ｙ", dist_size);	dist_p+=2;	break;
			case 'Z':	strncpy(dist_p, "Ｚ", dist_size);	dist_p+=2;	break;
			case '[':	strncpy(dist_p, "［", dist_size);	dist_p+=2;	break;
			case '\\':	strncpy(dist_p, "￥", dist_size);	dist_p+=2;	break;
			case ']':	strncpy(dist_p, "］", dist_size);	dist_p+=2;	break;
			case '^':	strncpy(dist_p, "＾", dist_size);	dist_p+=2;	break;
			case '_':	strncpy(dist_p, "  ", dist_size);	dist_p+=2;	break;
			case '`':	strncpy(dist_p, "‘", dist_size);	dist_p+=2;	break;
			case 'a':	strncpy(dist_p, "ａ", dist_size);	dist_p+=2;	break;
			case 'b':	strncpy(dist_p, "ｂ", dist_size);	dist_p+=2;	break;
			case 'c':	strncpy(dist_p, "ｃ", dist_size);	dist_p+=2;	break;
			case 'd':	strncpy(dist_p, "ｄ", dist_size);	dist_p+=2;	break;
			case 'e':	strncpy(dist_p, "ｅ", dist_size);	dist_p+=2;	break;
			case 'f':	strncpy(dist_p, "ｆ", dist_size);	dist_p+=2;	break;
			case 'g':	strncpy(dist_p, "ｇ", dist_size);	dist_p+=2;	break;
			case 'h':	strncpy(dist_p, "ｈ", dist_size);	dist_p+=2;	break;
			case 'i':	strncpy(dist_p, "ｉ", dist_size);	dist_p+=2;	break;
			case 'j':	strncpy(dist_p, "ｊ", dist_size);	dist_p+=2;	break;
			case 'k':	strncpy(dist_p, "ｋ", dist_size);	dist_p+=2;	break;
			case 'l':	strncpy(dist_p, "ｌ", dist_size);	dist_p+=2;	break;
			case 'm':	strncpy(dist_p, "ｍ", dist_size);	dist_p+=2;	break;
			case 'n':	strncpy(dist_p, "ｎ", dist_size);	dist_p+=2;	break;
			case 'o':	strncpy(dist_p, "ｏ", dist_size);	dist_p+=2;	break;
			case 'p':	strncpy(dist_p, "ｐ", dist_size);	dist_p+=2;	break;
			case 'q':	strncpy(dist_p, "ｑ", dist_size);	dist_p+=2;	break;
			case 'r':	strncpy(dist_p, "ｒ", dist_size);	dist_p+=2;	break;
			case 's':	strncpy(dist_p, "ｓ", dist_size);	dist_p+=2;	break;
			case 't':	strncpy(dist_p, "ｔ", dist_size);	dist_p+=2;	break;
			case 'u':	strncpy(dist_p, "ｕ", dist_size);	dist_p+=2;	break;
			case 'v':	strncpy(dist_p, "ｖ", dist_size);	dist_p+=2;	break;
			case 'w':	strncpy(dist_p, "ｗ", dist_size);	dist_p+=2;	break;
			case 'x':	strncpy(dist_p, "ｘ", dist_size);	dist_p+=2;	break;
			case 'y':	strncpy(dist_p, "ｙ", dist_size);	dist_p+=2;	break;
			case 'z':	strncpy(dist_p, "ｚ", dist_size);	dist_p+=2;	break;
			case '{':	strncpy(dist_p, "｛", dist_size);	dist_p+=2;	break;
			case '|':	strncpy(dist_p, "｜", dist_size);	dist_p+=2;	break;
			case '}':	strncpy(dist_p, "｝", dist_size);	dist_p+=2;	break;

			default:
				*dist_p = *src_p;
				dist_p++;
				*dist_p = '\0';
				break;
		}

		src_p++;
	}

	return;
}

//******************************************************************
// EUC文字列を、n byteに削除。文字列境界を見て良きに計らう。
// もし menu_font_metric が指定されており menu_font_metric_string が
// 指定されていれば、その値に従ってちょんぎる。
//******************************************************************
void euc_string_cut_n_length(unsigned char *euc_sentence,  unsigned int n)
{
	int	euc_flag = 0;
	unsigned char	*p, *prev_p;
	int total_len = 0;
	int size = 0;

	total_len = global_param.menu_font_metric * n;


	prev_p = p = euc_sentence;
	while (*p && total_len > 0) {
		if (*p >= sizeof(global_param.menu_font_metric_string)) {
			size = global_param.menu_font_metric;
		} else {
			char ch;
			ch = global_param.menu_font_metric_string[*p];
			if (ch < 'a' || ch > 'z') {
				size = global_param.menu_font_metric;
			} else {
				size = ch - 'a';
				if (size <= 0) {
					//debug_log_output("damn. old size: %d", size);
					size = global_param.menu_font_metric;
				}
			}
		}
		if (total_len < size) {
			if (euc_flag == 1) {
				*prev_p = '\0';
			} else {
				*p = '\0';
			}
			return ;
		}
		if (euc_flag == 0 && (( *p >= 0xA1 ) || ( *p == 0x8E ))) {
			// EUC-Code or 半角カタカナの上位
			euc_flag = 1;
		} else {
			euc_flag = 0;
		}
		total_len -= size;
		prev_p = p++;
	}

	debug_log_output("debug: rest %d", total_len);
	*p = '\0';

	return;
}
// **************************************************************************
// * PNGフォーマットファイルから、画像サイズを得る。
// **************************************************************************
void png_size(unsigned char *png_filename, unsigned int *x, unsigned int *y)
{
	int		fd;
	unsigned char	buf[255];
	ssize_t	read_len;

	*x = 0;
	*y = 0;

	fd = open(png_filename, O_RDONLY);
	if ( fd < 0 )
	{
		return;
	}

	// ヘッダ+サイズ(0x18byte)  読む
	memset(buf, 0, sizeof(buf));
	read_len = read(fd, buf, 0x18);
	if ( read_len == 0x18)
	{
		*x = 	(buf[0x10] << 24)	+
				(buf[0x11] << 16)	+
				(buf[0x12] << 8 )	+
				(buf[0x13]);

		*y = 	(buf[0x14] << 24) 	+
				(buf[0x15] << 16)	+
				(buf[0x16] << 8 )	+
				(buf[0x17]);
	}

	close( fd );


	return;
}


// **************************************************************************
// * GIFフォーマットファイルから、画像サイズを得る。
// **************************************************************************
void gif_size(unsigned char *gif_filename, unsigned int *x, unsigned int *y)
{
	int		fd;
	unsigned char	buf[255];
	ssize_t	read_len;

	*x = 0;
	*y = 0;

	fd = open(gif_filename, O_RDONLY);
	if ( fd < 0 )
	{
		return;
	}


	// ヘッダ+サイズ(10byte)  読む
	memset(buf, 0, sizeof(buf));
	read_len = read(fd, buf, 10 );
	if ( read_len == 10)
	{
		*x = buf[6] + (buf[7] << 8);
		*y = buf[8] + (buf[9] << 8);
	}

	close( fd );

	return;
}



// **************************************************************************
// * JPEGフォーマットファイルから、画像サイズを得る。
// **************************************************************************
void  jpeg_size(unsigned char *jpeg_filename, unsigned int *x, unsigned int *y)
{
	int		fd;
	unsigned char	buf[255];
	ssize_t		read_len;
	off_t		length;

	*x = 0;
	*y = 0;

	//debug_log_output("jpeg_size: '%s'.", jpeg_filename);


	fd = open(jpeg_filename,  O_RDONLY);
	if ( fd < 0 )
	{
		return;
	}

	while ( 1 )
	{
		// マーカ(2byte)  読む
		read_len = read(fd, buf, 2);
		if ( read_len != 2)
		{
			//debug_log_output("fraed() EOF.\n");
			break;
		}

		// Start of Image.
		if (( buf[0] == 0xFF ) && (buf[1] == 0xD8))
		{
			continue;
		}

		// Start of Frame 検知
		if (( buf[0] == 0xFF ) && ( buf[1] >= 0xC0 ) && ( buf[1] <= 0xC3 )) // SOF 検知
		{
			//debug_log_output("SOF0 Detect.");

			// sof データ読み込み
			memset(buf, 0, sizeof(buf));
			read_len = read(fd, buf, 0x11);
			if ( read_len != 0x11 )
			{
				debug_log_output("fraed() error.\n");
				break;
			}

			*y = (buf[3] << 8) + buf[4];
			*x = (buf[5] << 8) + buf[6];

			break;
		}

		// SOS検知
		if (( buf[0] == 0xFF ) && (buf[1] == 0xDA)) // SOS 検知
		{
			//debug_log_output("Start Of Scan.\n");

			// 0xFFD9 探す。
			while ( 1 )
			{
				// 1byte 読む
				read_len = read(fd, buf, 1);
				if ( read_len != 1 )
				{
					//debug_log_output("fraed() error.\n");
					break;
				}

				// 0xFFだったら、もう1byte読む
				if ( buf[0] == 0xFF )
				{
					buf[0] = 0;
					read(fd, buf, 1);

					// 0xD9だったら 終了
					if ( buf[0] == 0xD9 )
					{
						//debug_log_output("End Of Scan.\n");
						break;
					}
				}
			}
			continue;
		}


		// length 読む
		memset(buf, 0, sizeof(buf));
		read(fd, buf, 2);
		length = (buf[0] << 8) + buf[1];

		// length分とばす
		lseek(fd, length-2, SEEK_CUR );
	}

	close(fd);

	return;
}



// **************************************************************************
// * SJIS文字列の中から、codeをを含む文字(2byte文字)を、固定文字列で置換する。
// **************************************************************************
void sjis_code_thrust_replase(unsigned char *sentence, const unsigned char code)
{
	int i;
	unsigned char rep_code[2];

	// 置換する文字列。(SJISで'＊'）
	rep_code[0] = 0x81;
	rep_code[1] = 0x96;

	// 文字列長が、1byte以下なら、処理必要なし。
	if ( strlen(sentence) <= 1 )
	{
		return;
	}


	// 置換対象文字 捜索
	for ( i=1; sentence[i] != '\0' ;i++ )
	{
		// code にヒット？
		if ( sentence[i] == code )
		{
			// 1byte前が、SJIS 1byte目の範囲？(0x81〜0x9F、0xE0〜〜0xFC)
			if ((( sentence[i-1] >= 0x81 ) && (sentence[i-1] <= 0x9F)) ||
				(( sentence[i-1] >= 0xE0 ) && (sentence[i-1] <= 0xFC))		)
			{

				debug_log_output("SJIS Replase HIT!!!!");

				// 置換実行
				sentence[i-1] = rep_code[0];
				sentence[i  ] = rep_code[1];
			}
		}
	}

	return;

}


/* Cygwin等に strcasestr がないので strncasecmpを使って自作 */
char *my_strcasestr(const char *p1, const char *p2)
{
	size_t len;

	len = strlen(p2);
	if (len == 0) return (char*)p1;

	while (*p1) {
		if (!strncasecmp(p1, p2, len)) {
			return (char*)p1;
		}
		p1++;
	}
	return NULL;
}


char *path_sanitize(char *orig_dir, size_t dir_size)
{
	char *p;
	char *q, *dir, *alt;
	char *buf;
	size_t malloc_len;

	if (orig_dir == NULL) return NULL;

	malloc_len = strlen(orig_dir) * 2;
	buf = malloc(malloc_len);
	buf[0] = '\0';
	p = buf;

	dir = q = orig_dir;
	if((dir[0] != 0) && (dir[1] == ':')) {
		// Special handling for windows drive names
		// Skip over and retain the drive and colon
		*(p++) = *(q++);
		*(p++) = *(q++);
	}
	while (q != NULL) {
		dir = q;
		while ((*dir == '/') || (*dir == '\\')) dir ++;

		q = strchr(dir, '/');
		alt = strchr(dir, '\\');
		if( (q == NULL) || ((alt != NULL) && (alt<q)) ) {
			q = alt;
		}
		if (q != NULL) {
			*q++ = '\0';
		}

		if (!strcmp(dir, "..")) {
			p = strrchr(buf, '/');
			if (p == NULL) p = strrchr(buf, '\\');
			if (p == NULL) {
				free(buf);
				dir[0] = '\0';
				return NULL; //  not allowed.
			}
			*p = '\0';
		} else if (strcmp(dir, ".")) {
			p += snprintf(p, malloc_len - (p - buf), "/%s", dir);
		}
	}

	if (buf[0] == '\0') {
		strncpy(orig_dir, "/", dir_size);
	} else {
		strncpy(orig_dir, buf, dir_size);
	}
	free(buf);
	return orig_dir;
}

void send_printf(int fd, char *fmt, ...) {
	va_list ap;
	char linebuf[2048];

	va_start(ap, fmt);
	vsnprintf(linebuf, sizeof(linebuf), fmt, ap);
	va_end(ap);

	//printf("%s", linebuf);
	write(fd, linebuf, strlen(linebuf));
}

// hogehoge/abc/ddd/// -> hogehoge/abc/
char *get_parent_path(char *dst, char *src, size_t len)
{
	char *p;

	if (strlen(src) > len || len <= 0) {
		debug_log_output("get_parent_path: no enough space");
		strncpy(dst, "/", len);
		return dst;
	}
	// とりあえず全部コピー。
	strncpy(dst, src, len);

	// 一番後ろの '/' を削除
	for (p = dst + strlen(dst) - 1; p > dst && *p == '/'; p--) *p = '\0';

	p = strrchr(dst, '/');
	if (p == NULL || p == dst) {
		debug_log_output("get_parent_path: no / anymore..., i.e., root.");
		strncpy(dst, "/", len);
		return dst;
	}

	// 残った最後の '/' より後ろを削除
	*++p = '\0';

	return dst;
}
