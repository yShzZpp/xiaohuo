#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    ssize_t u32Size, u32DataLen;
    /* 构造查询包 */
    char aQuary[2048] = 
	"GET /otn/leftTicket/query?leftTicketDTO.train_date=2022-07-16&leftTicketDTO.from_station=SHH&leftTicketDTO.to_station=BJP&purpose_codes=ADULT HTTP/1.1\r\n"
	"Host: kyfw.12306.cn\r\n"
	"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:102.0) Gecko/20100101 Firefox/102.0\r\n"
	"Accept: */*\r\n"
	"Accept-Language: en-US,en;q=0.5\r\n"
	"Accept-Encoding: gzip, deflate, br\r\n"
	"If-Modified-Since: 0\r\n"
	"Cache-Control: no-cache\r\n"
	"X-Requested-With: XMLHttpRequest\r\n"
	/** "Connection: keep-alive\r\n" */
	"Connection: false\r\n"
	"Referer: https://kyfw.12306.cn/otn/leftTicket/init?linktypeid=dc\r\n"
	"Cookie: _uab_collina=165784922762669618964436; JSESSIONID=DB8377CDAB8EC92EC577593B10DFAE73; BIGipServerotn=4107731210.64545.0000; guidesStatus=off; highContrastMode=defaltMode; cursorStatus=off; RAIL_EXPIRATION=1658165248978; RAIL_DEVICEID=OG6NvPpvhHKLZpFZ-0cQoVF_5jQn_uemA62Ub4xm91VoIZxJYdZHeVjP8pjTndmzrC-Sjk0mjqTtv1Qyp6wy1dLuwVayDUVw3vTkU8v6_NhJsCyl_quOneqgr_wEIvxYK3Ui9IxtDZYqPi5q3ACqWxE2JoO1U1HZ; BIGipServerpassport=854065418.50215.0000; route=6f50b51faa11b987e576cdb301e545c4; _jc_save_fromStation=%u4E0A%u6D77%2CSHH; _jc_save_toStation=%u5317%u4EAC%2CBJP; _jc_save_fromDate=2022-07-16; _jc_save_toDate=2022-07-15; _jc_save_wfdc_flag=dc\r\n"
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

	struct hostent *pstHost = gethostbyname("kyfw.12306.cn");
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
	//加载SSL错误信息 
	SSL_load_error_strings();
	//添加SSL的加密/HASH算法 
	SSLeay_add_ssl_algorithms();
	//客户端，服务端选择SSLv23_server_method() 
	const SSL_METHOD* meth = SSLv23_client_method();
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


    /* 发送请求 */
    pcSendData = aQuary;
    u32DataLen = strlen(aQuary);
	ret = SSL_write(ssl,aQuary,u32DataLen);
	if(ret == -1)
	{
		printf("SSL_write 错误\n");
		return -1;
	}

    /** while(u32DataLen) */
    /** { */
	/**     write(1, pcSendData, u32DataLen); */
    /**     u32Size = send(fd,pcSendData, u32DataLen, 0); */
    /**     if (u32Size <= 0) */
    /**     { */
    /**         perror("send"); */
    /**         return 1; */
    /**     } */
    /**     u32DataLen -= u32Size; */
    /**     pcSendData += u32Size; */
    /** } */

    /* 获取响应 */
	uint32_t u32Total = 20240,u32Recv = 0;

	ret = SSL_read(ssl,aRecvData,u32Total);
	if(ret == -1)	
	{
		printf("ssl read error\n");
		return -1;
	}
	aRecvData[ret -1 ] = 0;
	printf("[%d]recv:\n%s",ret,aRecvData);

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
	for(uint32_t i = 0;i < ret ; i++)
	{
		printf("02%x",(uint8_t)aRecvData[i]);
	}
	printf("\n");

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
	for(uint32_t i = 0;i < ret ; i++)
	{
		printf("02%x",(uint8_t)aRecvData[i]);
	}
	printf("\n");


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
