#include "../inc/TRAIN_HTTPS.h"
#include <string.h>


const char g_aQuary[2048] = 
	"Host: kyfw.12306.cn\r\n"
	"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:102.0) Gecko/20100101 Firefox/102.0\r\n"
	"Accept: */*\r\n"
	"Accept-Language: en-US,en;q=0.5\r\n"
	"If-Modified-Since: 0\r\n"
	"Cache-Control: no-cache\r\n"
	"X-Requested-With: XMLHttpRequest\r\n"
	/** "Connection: keep-alive\r\n" */
	"Connection: false\r\n"
	"Referer: https://kyfw.12306.cn/otn/leftTicket/init?linktypeid=dc\r\n"
	"Cookie: _uab_collina=165784922762669618964436; JSESSIONID=352173FCC3BB17B983E22D16321260A0; guidesStatus=off; highContrastMode=defaltMode; cursorStatus=off; RAIL_EXPIRATION=1658165248978; RAIL_DEVICEID=OG6NvPpvhHKLZpFZ-0cQoVF_5jQn_uemA62Ub4xm91VoIZxJYdZHeVjP8pjTndmzrC-Sjk0mjqTtv1Qyp6wy1dLuwVayDUVw3vTkU8v6_NhJsCyl_quOneqgr_wEIvxYK3Ui9IxtDZYqPi5q3ACqWxE2JoO1U1HZ; _jc_save_fromStation=%%u4E0A%%u6D77%%2CSHH; _jc_save_toStation=%%u5317%u4EAC%%2CBJP; _jc_save_fromDate=2022-07-16; _jc_save_toDate=2022-07-15; _jc_save_wfdc_flag=dc; BIGipServerpassport=770179338.50215.0000; route=6f50b51faa11b987e576cdb301e545c4; BIGipServerotn=1960378634.50210.0000\r\n"
	"Sec-Fetch-Dest: empty\r\n"
	"Sec-Fetch-Mode: cors\r\n"
	"Sec-Fetch-Site: same-origin\r\n"
	"\r\n";

/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int TRAIN_HTTPS_Get_Quest_Data(char *pcData,char *pcTime,char *pcFrom,char *pcTo)
{
	
    char *pcSendData = malloc(2048);
	sprintf(pcSendData,
	"GET /otn/leftTicket/query?leftTicketDTO.train_date=%s&leftTicketDTO.from_station=%s&leftTicketDTO.to_station=%s&purpose_codes=ADULT HTTP/1.1\r\n"
	,pcTime,pcFrom,pcTo);
	strcat(pcSendData, g_aQuary);


    ssize_t u32Size, u32DataLen;
    /* 构造查询包 */
	/* openssl*/

	SSL_library_init();

	//添加SSL的加密/HASH算法 
	/** SSLeay_add_ssl_algorithms(); */
	OpenSSL_add_all_algorithms();
	//加载SSL错误信息 
	SSL_load_error_strings();
	//客户端，服务端选择SSLv23_server_method() 
	const SSL_METHOD* meth = SSLv23_client_method();
	/** const SSL_METHOD* meth = TLSv1_2_client_method(); */
	//建立新的SSL上下文 
	SSL_CTX* ctx = SSL_CTX_new(meth);
	if (ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
		printf("SSL_CTX_new错误\n");
		return -1;
	}


    int fd;
    struct sockaddr_in stClientAddr;

    /* 创建一个套接字用来连接服务器 */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
		perror("socket");
		return -1;
    }

	struct hostent *pstHost = gethostbyname("kyfw.12306.cn");
	if(pstHost == NULL || pstHost->h_addr_list[0] == NULL)
	{
		printf("获取ip失败\n");
		return 0;
	}

    /* socket连接到服务器 */
    stClientAddr.sin_family = AF_INET;
	stClientAddr.sin_port = htons(443);
	stClientAddr.sin_addr = *(struct in_addr *)pstHost->h_addr_list[0];
	/** printf("ip:%s\n",inet_ntoa(stClientAddr.sin_addr)); */
    if (connect(fd, (struct sockaddr *)&stClientAddr, sizeof(stClientAddr)))
    {
        perror("connect");
        close(fd);
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

	
    /* 发送请求 */
    u32DataLen = strlen(pcSendData);
	ret = SSL_write(ssl,pcSendData,u32DataLen);
	if(ret == -1)
	{
		printf("SSL_write 错误\n");
		return -1;
	}
	//释放空间
	free(pcSendData);

    /* 获取响应 */
	uint32_t u32Recv = 0;
	char aTempBuff[1024];
	memset(aTempBuff,0,1024);

	ret = SSL_read(ssl,aTempBuff,MAX_DATA_SIZE);
	if(ret == -1)	
	{
		printf("ssl read error\n");
		return -1;
	}
	aTempBuff[ret -1 ] = 0;
	/** printf("[%d]recv:\n%s",ret,aTempBuff); */
	for( uint32_t i = 0 ; i < ret; i++ )
	{
		if( i > 4 && aTempBuff[i-4] == 't' && aTempBuff[i-3] == 'h' && aTempBuff[i-2] == ':' )
		{
			sscanf(&aTempBuff[i],"%d",&u32Recv);	
		}
	}
	/** printf("%d\n",u32Recv); */

	//清零
	memset(pcData,0,MAX_DATA_SIZE);

	int time = 0;
	ret = 0;
	while(ret < u32Recv)
	{
		time++;
		/** printf("can read %d\n",u32Recv - ret); */
		ret += SSL_read(ssl,(pcData + ret),u32Recv - ret);
		if(ret == -1)	
		{
			printf("ssl read error\n");
			return -1;
		}

		/** printf("\ndata_%d[%d]:\n",time,ret); */

		if(ret == u32Recv)
		{
			break;
		}
	}

	/** pcData[u32Recv] = '\n'; */

    close(fd);
	//关闭SSL套接字 
	SSL_shutdown(ssl);
	//释放SSL套接字 
	SSL_free(ssl);
	//释放SSL会话环境 
	SSL_CTX_free(ctx);


    return 0;
}


