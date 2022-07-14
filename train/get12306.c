#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    ssize_t u32Size, u32DataLen;
    /* 构造查询包 */
    char aQuary[2048] = 
	"GET /otn/leftTicket/query?leftTicketDTO.train_date=2022-07-15&leftTicketDTO.from_station=SHH&leftTicketDTO.to_station=GZQ&purpose_codes=ADULT HTTP/1.1 \r\n"
	"Host: kyfw.12306.cn\r\n"
	"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:102.0) Gecko/20100101 Firefox/102.0\r\n"
	"Accept: */*\r\n"
	"Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2\r\n"
	"Accept-Encoding: gzip, deflate, b\r\n"
	"Referer: https://kyfw.12306.cn/otn/leftTicket/init?linktypeid=dc\r\n"
	"Connection: keep-alive\r\n"
	"Cookie: _uab_collina=165778753760215014419609; JSESSIONID=6E22D5A7932ED7EC499E537FD26E6FDA; BIGipServerotn=619184394.50210.0000; RAIL_EXPIRATION=1658047961140; RAIL_DEVICEID=PqJafDoAVmOKag3_CTo6DyxzN4G2Q7zgTAJMWqraWV_HcIgSRoQxfILn1XgA1OkyHXtceb7J8akyQ0FHlCPRngmlzpDIOn68Ze6ztGz13W9CjFbKFi3KYGHBXYiNWXCI4vy2VRPy3p5GkydFZuzKacyk9mWbtRnC; BIGipServerpassport=820510986.50215.0000; guidesStatus=off; highContrastMode=defaltMode; cursorStatus=off; route=c5c62a339e7744272a54643b3be5bf64; _jc_save_fromStation=%u4E0A%u6D77%2CSHH; _jc_save_toStation=%u5317%u4EAC%2CBJP; _jc_save_fromDate=2022-07-15; _jc_save_toDate=2022-07-14; _jc_save_wfdc_flag=dc\r\n"
	"Sec-Fetch-Dest: empty \r\n"
	"Sec-Fetch-Mode: no-cors\r\n"
	"Sec-Fetch-Site: same-origin\r\n"
	"If-Modified-Since: 0\r\n"
	"Cache-Control: no-cache\r\n"
	"X-Requested-With: XMLHttpRequest \r\n"
	"Pragma: no-cache \r\n"
	"\r\n";

	printf("head:\n%s",aQuary);


    int fd;
    struct sockaddr_in stClientAddr;
    char aRecvData[10240];
    const char *pcSendData;

    /* 创建一个套接字用来连接服务器 */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket");
        return 1;
    }

    /* 连接到服务器 */
    stClientAddr.sin_family = AF_INET;
    stClientAddr.sin_port = htons(80);
	inet_pton(AF_INET,"27.148.151.20",&stClientAddr.sin_addr);
    if (connect(fd, (struct sockaddr *)&stClientAddr, sizeof(stClientAddr)))
    {
        perror("connect");
        close(fd);
        return 1;
    }


    /* 发送请求 */
    pcSendData = aQuary;
    u32DataLen = strlen(aQuary);
    while(u32DataLen)
    {
        u32Size = send(fd,pcSendData, u32DataLen, 0);
        if (u32Size <= 0)
        {
            perror("send");
            return 1;
        }
        u32DataLen -= u32Size;
        pcSendData += u32Size;
    }

    /* 获取响应 */
	uint32_t u32Total = 10240,u32Recv = 0;
    while(u32Total > u32Recv)
    {
        ssize_t ret = recv(fd, aRecvData, u32Total - u32Recv, 0);
        if (ret == 0)
        {
            break;
        }
        else if (ret < 0)
        {
            perror("recv\n");
            close(fd);
            return 1;
        }
		u32Recv += ret;
    }
    close(fd);
	printf("recv:\n%s\n",aRecvData);
    return 0;
}
