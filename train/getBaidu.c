#include <stddef.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>


void ShowCerts(SSL* ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

int main(int argc, char **argv)
{
    ssize_t u32Size, u32DataLen;
    /* 构造查询包 */
    char aQuary[2048*4] = 
	"GET /click.gif?type=doc&id=1ecc8c984bfe04a1b0717fd5360cba1aa8118ca4&bool=0&page=new_index&pid=1&bid=1&fr=4&act_id=102459&url=https%3A%2F%2Fwenku.baidu.com%2F&refer=https%3A%2F%2Fwww.baidu.com%2Fother.php%3Fsc.Kf0000j6DGaYOYKEZ0cpiRJS4ClInYZsYfpvMoUj00fpjFDhX9jnON6YxDujm64DO3O5y2WGt9nL3LU09CzL46wgcn2Gonwa78BOaAgfAQXyiNb4Ivg6uHBwvx-Cr2C2JDbXvFCgrQ9U8nWNsaz3dztvUx1dIUA4MTVNfEh6B636wW7Co3bShepPWDHWNyWT52MAoeA1KCj8aBlPCTUGpvpRxdln.7D_NR2Ar5Od663DkI8jzEukmD2N9h9moovurs0.TLFWgv-b5HDkrfK1ThPGujYknHb0THY0IAYq_PhvzneDESc0IgP-T-qYXgK-5H00mywxIZ-suHYkn0KLULFb5UjuV_WrkQOB0ZFWIWYs0ZNzU7qGujYkPHn1nWn4rj6d0Addgv-b5H00UNqGujYs0AdYTAkxpyfqn0KsI-qGujYs0A-bm1dcHbc0TA-b5Hf0mv-b5Hb10APzm1YznWf1%26ck%3D6732.3.75.279.162.256.150.188%26dt%3D1657872161%26wd%3D%25E7%2599%25BE%25E5%25BA%25A6%25E6%2596%2587%25E5%25BA%2593%26tpl%3Dtpl_12273_25897_22126%26l%3D1533239885%26us%3DlinkName%253D%2525E6%2525A0%252587%2525E9%2525A2%252598-%2525E4%2525B8%2525BB%2525E6%2525A0%252587%2525E9%2525A2%252598%2526linkText%253D%2525E7%252599%2525BE%2525E5%2525BA%2525A6%2525E6%252596%252587%2525E5%2525BA%252593-%2525E8%2525AE%2525A9%2525E6%2525AF%25258F%2525E4%2525B8%2525AA%2525E4%2525BA%2525BA%2525E5%2525B9%2525B3%2525E7%2525AD%252589%2525E5%25259C%2525B0%2525E6%25258F%252590%2525E5%25258D%252587%2525E8%252587%2525AA%2525E6%252588%252591%2526linkType%253D&t=1657872207209&ie=utf-8 HTTP/1.1 \r\n"
	"Host: wkctj.baidu.com\r\n"
	"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:102.0) Gecko/20100101 Firefox/102.0\r\n"
	"Accept: image/avif,image/webp,*/*\r\n"
	"Accept-Language: en-US,en;q=0.5\r\n"
	"Accept-Encoding: gzip, deflate, br\r\n"
	"If-Modified-Since: 0\r\n"
	"Cache-Control: no-cache\r\n"
	"X-Requested-With: XMLHttpRequest\r\n"
	/** "Connection: keep-alive\r\n" */
	"Connection: false\r\n"
	"Referer: https://wenku.baidu.com/\r\n"
	/** "Cookie: _uab_collina=165784922762669618964436; JSESSIONID=DB8377CDAB8EC92EC577593B10DFAE73; BIGipServerotn=4107731210.64545.0000; guidesStatus=off; highContrastMode=defaltMode; cursorStatus=off; RAIL_EXPIRATION=1658165248978; RAIL_DEVICEID=OG6NvPpvhHKLZpFZ-0cQoVF_5jQn_uemA62Ub4xm91VoIZxJYdZHeVjP8pjTndmzrC-Sjk0mjqTtv1Qyp6wy1dLuwVayDUVw3vTkU8v6_NhJsCyl_quOneqgr_wEIvxYK3Ui9IxtDZYqPi5q3ACqWxE2JoO1U1HZ; BIGipServerpassport=854065418.50215.0000; route=6f50b51faa11b987e576cdb301e545c4; _jc_save_fromStation=%u4E0A%u6D77%2CSHH; _jc_save_toStation=%u5317%u4EAC%2CBJP; _jc_save_fromDate=2022-07-16; _jc_save_toDate=2022-07-15; _jc_save_wfdc_flag=dc\r\n" */
	"Sec-Fetch-Dest: empty\r\n"
	"Sec-Fetch-Mode: cors\r\n"
	"Sec-Fetch-Site: same-origin\r\n"
	"\r\n";

	/** printf("head:\n%s",aQuary); */


    int fd;
    struct sockaddr_in stClientAddr;
    char aRecvData[1024*1024];
    const char *pcSendData;

    /* 创建一个套接字用来连接服务器 */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return 1;
    }

	struct hostent *pstHost = gethostbyname("wkctj.baidu.com");
	if(pstHost == NULL || pstHost->h_addr_list[0] == NULL)
	{
		printf("获取ip失败\n");
		return 0;
	}

    /* 连接到服务器 */
    stClientAddr.sin_family = AF_INET;
    /** stClientAddr.sin_port = htons(80); */
	stClientAddr.sin_port = htons(443);
	stClientAddr.sin_addr = *(struct in_addr *)pstHost->h_addr_list[0];
	/** inet_pton(AF_INET,"27.148.151.20",&stClientAddr.sin_addr); */
    if (connect(fd, (struct sockaddr *)&stClientAddr, sizeof(stClientAddr)))
    {
        perror("connect");
        close(fd);
        return 1;
    }

	/* openssl*/

	SSL_library_init();

	//添加SSL的加密/HASH算法 
	/** SSLeay_add_ssl_algorithms(); */
	OpenSSL_add_all_algorithms();
	//加载SSL错误信息 
	SSL_load_error_strings();
	//客户端，服务端选择SSLv23_server_method() 
	/** const SSL_METHOD* meth = SSLv23_client_method(); */
	const SSL_METHOD* meth = TLSv1_2_client_method();
	//建立新的SSL上下文 
	SSL_CTX* ctx = SSL_CTX_new(meth);
	if (ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		printf("SSL_CTX_new错误\n");
		return -1;
	}

	//建立SSL
	int ret;
	SSL* ssl = SSL_new(ctx);
	if (ssl == NULL)
	{
		printf("新建SSL错误\n");
		return -1;
	}
	//将SSL与TCP SOCKET 连接 
	SSL_set_fd(ssl,fd);
	//SSL连接 
	ret = SSL_connect(ssl);
	if (ret == -1)
	{
		printf("SSL 连接错误\n");
		return -1;
	}

	printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
	ShowCerts(ssl);


    /* 发送请求 */
    pcSendData = aQuary;
    u32DataLen = strlen(aQuary);
	ret = SSL_write(ssl,aQuary,u32DataLen);
	if(ret == -1)
	{
		printf("SSL_write 错误\n");
		return -1;
	}

    /* 获取响应 */
	uint32_t u32Total = 20240;size_t u32Recv = 0;

	ret = SSL_read(ssl,aRecvData,u32Total);
	if(ret == -1)	
	{
		printf("ssl read error\n");
		return -1;
	}
	aRecvData[ret -1 ] = 0;
	printf("[%d]recv:\n%s",ret,aRecvData);

	//读取数据
	memset(aRecvData,0,u32Total);
	ret = SSL_read(ssl,aRecvData,u32Total);
	if(ret == -1)	
	{
		printf("ssl read error\n");
		return -1;
	}
	aRecvData[ret -1 ] = 0;
	printf("\n========data_1[%d]:\n",ret);
	/** write(1,aRecvData,ret); */
	/** for(uint32_t i = 0;i < 200; i++) */
	/** { */
	/**     printf("02%x",(uint8_t)aRecvData[i]); */
	/** } */
	/** printf("\n-%s-\n",aRecvData); */

	//读取数据
	memset(aRecvData,0,u32Total);
	ret = SSL_read(ssl,aRecvData,u32Total);
	if(ret == -1)	
	{
		printf("ssl read error\n");
		return -1;
	}
	aRecvData[ret -1 ] = 0;
	printf("\n========data_2[%d]:\n",ret);
	/** write(1,aRecvData,ret); */
	/** for(uint32_t i = 0;i < 200; i++) */
	/** { */
	/**     printf("02%x",(uint8_t)aRecvData[i]); */
	/** } */
	/** printf("\n%s\n",aRecvData); */


	//关闭SSL套接字 
	SSL_shutdown(ssl);
	//释放SSL套接字 
	SSL_free(ssl);
	//释放SSL会话环境 
	SSL_CTX_free(ctx);

    /** while(u32Total > u32Recv) */
    /** { */
    /**     ssize_t ret = recv(fd, aRecvData, u32Total - u32Recv, 0); */
    /**     if (ret == 0) */
    /**     { */
    /**         break; */
    /**     } */
    /**     else if (ret < 0) */
    /**     { */
    /**         perror("recv\n"); */
    /**         close(fd); */
    /**         return 1; */
    /**     } */
	/**     u32Recv += ret; */
	/**     printf("recv:\n%s\n",aRecvData); */
    /** } */
    close(fd);
    return 0;
}
