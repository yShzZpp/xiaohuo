#include <openssl/bioerr.h>
#include <stddef.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

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
	int ret;
    ssize_t u32Size, u32DataLen;
    /* 构造查询包 */
    char aQuary[2048] = 
	"GET / HTTP/1.1\r\n"
	"Host: www.baidu.com:443\r\n"
	"Accept: */*\r\n"
	"X-Requested-With: XMLHttpRequest\r\n"
	/** "Connection: keep-alive\r\n" */
	"Connection: false\r\n"
	"\r\n";

	/* openssl*/

	SSL_library_init();

	//添加SSL的加密/HASH算法 
	/** SSLeay_add_ssl_algorithms(); */
	SSLeay_add_ssl_algorithms();
	OpenSSL_add_all_algorithms();
	//加载SSL错误信息 
	SSL_load_error_strings();
	ERR_load_BIO_strings();


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

	//验证服务端的证书
	/** SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL); */

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

	struct hostent *pstHost = gethostbyname("www.baidu.com");
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
	printf("ip:%s\n",inet_ntoa(stClientAddr.sin_addr));
	/** inet_pton(AF_INET,"125.77.131.11",&stClientAddr.sin_addr); */
    if (connect(fd, (struct sockaddr *)&stClientAddr, sizeof(stClientAddr)))
    {
        perror("connect");
        close(fd);
        return 1;
    }

	//建立SSL
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
	ret = SSL_write(ssl,pcSendData,u32DataLen);
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
	write(1,aRecvData,ret);
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
	write(1,aRecvData,ret);
	/** for(uint32_t i = 0;i < 200; i++) */
	/** { */
	/**     printf("02%x",(uint8_t)aRecvData[i]); */
	/** } */
	/** printf("\n%s\n",aRecvData); */

	sleep(1);

    close(fd);
	//关闭SSL套接字 
	SSL_shutdown(ssl);
	//释放SSL套接字 
	SSL_free(ssl);
	//释放SSL会话环境 
	SSL_CTX_free(ctx);

    return 0;
}
