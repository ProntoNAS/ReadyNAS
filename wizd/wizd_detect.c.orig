// ==========================================================================
//code=EUC	tab=4
//
// wizd:	MediaWiz Server daemon.
//
// 		wizd_detect.c
//											$Revision: 1.23 $
//											$Date: 2006/05/20 04:54:31 $
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
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>


#include "wizd.h"


#define		SSDP_IP_ADDRESS		"239.255.255.250"
#define		SSDP_PORT			(1900)

#define		SSDP_CHECK_KEY		"Server"
#define		SSDP_CHECK_VALUE	"Syabas myiBox"

#define debug_log_output (void)

// =============================================================
// MediaWiz検出部
// =============================================================
void	server_detect()
{
    int    ret;

	int		ssdp_socket;	// SSDP 待ち受けSocket
    struct 	sockaddr_in		ssdp_server_addr;		// サーバ側ソケットアドレス構造体
    struct 	sockaddr_in		ssdp_client_addr;		// 受信ソケットアドレス構造体
	struct	ip_mreq			ssdp_mreq;				// マルチキャスト設定

	int		ssdp_client_addr_len = sizeof(ssdp_client_addr);


	int		sock_opt_val;
	int		recv_len;

	int						regist_socket;
	struct 	sockaddr_in		connect_addr, self_addr;
	int						self_addr_len;
	int		flag_media_wiz;

    unsigned char	recv_buf[1024*2];
	unsigned char	send_buf[1024*2];
	unsigned char	work_buf[1024];

	unsigned char	line_buf[1024];
	unsigned char	ssdp_check_key_buf[1024];
	unsigned char	*work_p;


	unsigned char	self_ip_address[32];

	debug_log_output("AutoDetect Process start.\n");

    // =============================
    // ソケット生成
    // =============================
	ssdp_socket = socket(AF_INET, SOCK_DGRAM, 0 );
	if ( ssdp_socket < 0 ) // ソケット生成失敗チェック
	{
		debug_log_output("AutoDetect: socket() error.");
		exit( 1 );
    }

    // REUSEADDR 設定
    sock_opt_val = 1;
    setsockopt(ssdp_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&sock_opt_val, sizeof(sock_opt_val));


    // ===========================================
    // ソケットアドレス構造体に値をセット
    // ===========================================
	memset( (char *)&ssdp_server_addr, 0, sizeof(ssdp_server_addr) );
    ssdp_server_addr.sin_family = AF_INET;
    ssdp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ssdp_server_addr.sin_port = htons(SSDP_PORT);


    // =============================
    // bind 実行
    // =============================
    ret = bind(ssdp_socket, (struct sockaddr *)&ssdp_server_addr, sizeof(ssdp_server_addr));
    if ( ret < 0 ) // bind 失敗チェック
    {
		debug_log_output("AutoDetect: bind() error.");
		exit( 1 );
    }


	// ===============================
	// ソケットにオプションをセット
	// ===============================


	// SSDPマルチキャスト メンバシップ設定
	ssdp_mreq.imr_multiaddr.s_addr = inet_addr(SSDP_IP_ADDRESS);

	if ( strlen(global_param.auto_detect_bind_ip_address) == 0 )
	{
		debug_log_output("SSDP Listen:INADDR_ANY");
		ssdp_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		debug_log_output("SSDP Listen:%s", global_param.auto_detect_bind_ip_address);
		ssdp_mreq.imr_interface.s_addr = inet_addr(global_param.auto_detect_bind_ip_address);
	}
	setsockopt(ssdp_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ssdp_mreq, sizeof(ssdp_mreq)); // メンバシップ


	// =====================
	// メインループ
	// =====================
    while ( 1 )
    {
		// SSDPパケット受信
		memset(recv_buf, '\0', sizeof(recv_buf));
   		recv_len = recvfrom(ssdp_socket, &recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&ssdp_client_addr, &ssdp_client_addr_len );

		debug_log_output("=============== SSDP Receive. ================");

		// ssdp_client_addr 情報表示
		debug_log_output("SSDP client addr = %s\n", inet_ntoa(ssdp_client_addr.sin_addr) );
		debug_log_output("SSDP client port = %d\n", ntohs(ssdp_client_addr.sin_port) );

		// 受け取った内容を表示
		debug_log_output("SSDP recv_len=%d, recv_buf=(ry ", recv_len);
		//debug_log_output("SSDP recv = '%s'", recv_buf );



		// ======================
		// MediaWizかチェック。
		// ======================
		flag_media_wiz = 0;


		debug_log_output("");
		debug_log_output("--- SSDP ReceiveData Info Start. -----------------------------");

		work_p = recv_buf;
		while ( work_p != NULL )
		{
			// 受信バッファから、１行切り出す。
			work_p = buffer_distill_line(work_p, line_buf, sizeof(line_buf) );
			debug_log_output("recv:'%s'", line_buf );

			//  ':'の前(Key)を切り出し
			strncpy(ssdp_check_key_buf, line_buf, sizeof(ssdp_check_key_buf) );
			cut_after_character( ssdp_check_key_buf, ':');

			// Key が "Server" かチェック
			if ( strcasecmp(ssdp_check_key_buf, SSDP_CHECK_KEY ) == 0 ) 
			{
				// valueに SSDP_CHECK_VALUE が含まれるかチェック。
				cut_before_character(line_buf, ':');
				if ( strstr(line_buf, SSDP_CHECK_VALUE) != NULL )
				{
					// 含まれてた。MediaWizである。
					debug_log_output( "HIT!!  This is MediaWiz!!", SSDP_CHECK_VALUE );

					flag_media_wiz = 1;
					break;
				}
			}
		}
		debug_log_output("--- SSDP ReceiveData Info End. -----------------------------");
		debug_log_output("");

		// MediaWizじゃなかった。なにもしない。
		if ( flag_media_wiz == 0 ) 
		{
			debug_log_output("Not MeidaWiz.");
			debug_log_output("=============== SSDP process end. ================ \n");
			continue;
		}



		// ======================
		// 自動登録実行
		// ======================


	    // ----------------------------
	    // 登録用ソケット生成
	    // ----------------------------
		regist_socket = socket(AF_INET, SOCK_STREAM, 0 );
		if ( regist_socket < 0 ) // ソケット生成失敗チェック
		{
			debug_log_output("AutoDetect: socket() error.");
			continue;
	    }



		// ----------------------
		// MediaWizにCONNECT実行
		// ----------------------
		memset(&connect_addr, 0, sizeof(connect_addr) );
		connect_addr.sin_family = AF_INET;
		connect_addr.sin_addr = ssdp_client_addr.sin_addr;
		connect_addr.sin_port = ssdp_client_addr.sin_port;

		ret = connect(regist_socket, (struct sockaddr *)&connect_addr, sizeof(connect_addr));
		if ( regist_socket < 0 )
		{
			debug_log_output("AutoDetect: connect() error.");
			continue;
		}

		// ------------------------------------
		// MediaWizと接続した自分自身のIPをGet.
		// ------------------------------------

		memset(&self_addr, 0, sizeof(self_addr) );
		self_addr_len = sizeof(self_addr);
		ret = getsockname(regist_socket, (struct sockaddr *)&self_addr, &self_addr_len );
		if ( ret < 0 ) // getsockname() エラーチェック
		{
			debug_log_output("getsockname() error. ret = %d", ret );
			continue;
		}

		strncpy( self_ip_address, inet_ntoa(self_addr.sin_addr), sizeof(self_ip_address) );
		debug_log_output("SSDP self addr = %s\n", self_ip_address );




		// --------------------------
		// MediaWizへの送信内容生成。
		// --------------------------
		debug_log_output("to MediaWiz Registration Start.");
		
		memset(send_buf, '\0', sizeof(send_buf));
		snprintf(send_buf, sizeof(send_buf), 
				"GET /myiBoxUPnP/description.xml?POSTSyabasiBoxURLpeername=%s&peeraddr=%s:%d& HTTP/1.1\r\n",
					global_param.server_name, 
					self_ip_address, 
					global_param.server_port 			);

		snprintf(work_buf, sizeof(work_buf), "User-Agent: %s\r\n", SERVER_NAME);
		strncat(send_buf, work_buf, sizeof(send_buf) - strlen(send_buf));
		snprintf(work_buf, sizeof(work_buf), "Host: %s:%d\r\n", inet_ntoa(ssdp_client_addr.sin_addr), ntohs(ssdp_client_addr.sin_port) );
		strncat(send_buf, work_buf, sizeof(send_buf) - strlen(send_buf));
		strncat(send_buf, "\r\n", sizeof(send_buf) - strlen(send_buf));

		debug_log_output("%s", send_buf);


		// --------------------------
		// 送信実行
		// --------------------------
		send(regist_socket, send_buf, strlen(send_buf), 0);

		// --------------------------
		// 返信を受信する。
		// --------------------------
		while( 1 ) 
		{
			recv_len = recv(regist_socket, recv_buf, sizeof(recv_buf), 0);
			if ( recv_len <= 0 ) 
				break;
			work_p = recv_buf;
			while ((work_p = buffer_distill_line(work_p, line_buf, sizeof(line_buf) )) != NULL) {
				debug_log_output("recv: %s", line_buf);
			}
		}


		// --------------------------
		// 登録用Socketをclose.
		// --------------------------
		close(regist_socket); 


		debug_log_output("=============== SSDP process end. ================ \n");

    }


	return;
}


