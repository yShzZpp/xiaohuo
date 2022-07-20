#include <stddef.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../inc/TRAIN_cJSON.h"
#include "../inc/TRAIN_HTTPS.h"
#include "../inc/TRAIN_Main.h"
#include "../inc/TRAIN_Get_Station_Map.h"

void MAIN_Print_Data(char *pcData)
{
	uint8_t u8Len = strlen(pcData);
	/** printf("\n%d\n",u8Len); */
	//1个字
	if(u8Len == 3)
	{
		printf("  \033[032m%3s\033[0m  |",pcData);	
	}
	//2个字
	else if(u8Len == 6)
	{
		printf("   \033[032m%6s\033[0m   |",pcData);	
	}
	//3个字
	else if(u8Len == 9)
	{
		printf("   \033[032m%9s\033[0m |",pcData);	
	}
	//4个字
	else if(u8Len == 12)
	{
		printf("  \033[032m%12s\033[0m|",pcData);	
	}
	else if(u8Len == 15)
	{
		printf("\033[032m%15s\033[0m|",pcData);	
	}
	else 
	{
		printf("%10s|","");
	}
}

/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int MAIN_Parse_Train_JSON(char *pcData)
{
	if(pcData == NULL)
	{
		return 0;
	}

	cJSON *cjRoot = cJSON_Parse(pcData);
	if(cjRoot == NULL || cjRoot->type != cJSON_Object)
	{
		printf("不是json obj\n");
		return -1;
	}

	cJSON *cjData = cJSON_GetObjectItem(cjRoot, "data");
	if(cjData == NULL || cjData->type != cJSON_Object)
	{
		printf("data数据有误\n");
		return -1;
	}

	cJSON *cjResultArray = cJSON_GetObjectItem(cjData, "result");
	if(cjResultArray == NULL || cjResultArray->type != cJSON_Array)
	{
		printf("result数据有误\n");
		return -1;
	}

	//共有信息
	char aName[10],aStart[30],aEnd[30],aFrom[30],aTo[30],aStartTime[20],aEndTime[20],aAllTime[20];
	//高铁
	char aYiDeng[10],aErDeng[10],aTeDeng[10],aDongWo[10];
	//绿皮
	char aGaoRuanWo[10],aYiRuanWo[10],aYingWo[10],aRuanZuo[10],aYingZuo[10],aWuZuo[10];

	char aTempBuff[MAX_DATA_SIZE];
	//获取所需信息
	uint32_t u32ResultSize = cJSON_GetArraySize(cjResultArray);
	printf("有%d条车辆信息\n",u32ResultSize);
	printf("+---------------------------------------------------------------------------------------------------------------------------+\n");
	printf("|序号| 车次 |  始发站  |  终点站  | 查询起点 | 查询终点 | 发车时间 | 抵达时间 |   历时  |              余票信息             |\n");
	printf("|---------------------------------------------------------------------------------------------------------------------------|\n");
	for(uint32_t i = 0 ; i < u32ResultSize ; i++)
	{

		cJSON *cjTemp = cJSON_GetArrayItem(cjResultArray, i);
		if(cjTemp != NULL && cjTemp->type == cJSON_String)
		{

			//清空信息
			bzero(aName,sizeof(aName));bzero(aStart,sizeof(aStart));bzero(aEnd,sizeof(aEnd));
			bzero(aFrom,sizeof(aFrom));bzero(aTo,sizeof(aTo));bzero(aStartTime,sizeof(aStartTime));
			bzero(aEndTime,sizeof(aEndTime)); bzero(aAllTime,sizeof(aAllTime));

			bzero(aYiDeng,sizeof(aYiDeng));bzero(aErDeng,sizeof(aErDeng));bzero(aTeDeng,sizeof(aTeDeng)); bzero(aDongWo,sizeof(aDongWo));

			bzero(aGaoRuanWo,sizeof(aGaoRuanWo));bzero(aYiRuanWo,sizeof(aYiRuanWo)); bzero(aYingWo,sizeof(aYingWo));
			bzero(aRuanZuo,sizeof(aRuanZuo)); bzero(aYingZuo,sizeof(aYingZuo)); bzero(aWuZuo,sizeof(aWuZuo));

			uint32_t u32Len = strlen(cjTemp->valuestring);
			strcpy(aTempBuff,cjTemp->valuestring);

			uint32_t u32Index = 0;
			for(uint8_t j = 0 ; j < 48 ; j ++)
			{
				if(j == 3 )				//车次名称
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aName);
				}
				else if(j == 4)			//始发站
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aStart);
				}
				else if(j == 5)			//终点站
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aEnd);
				}
				else if(j == 6)			//来站
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aFrom);
				}
				else if(j == 7)			//去站
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aTo);
				}
				else if(j == 8)			//发车时间
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aStartTime);
				}
				else if(j == 9)			//抵达时间
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aEndTime);
				}
				else if(j == 10)		//历时
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aAllTime);
				}
				else if(j == 32 && aName[0] == 'G')		//商务特等
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aTeDeng);
				}
				else if(j == 31)		//一等
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aYiDeng);
				}
				else if(j == 30	)		//二等
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aErDeng);
				}

				//绿皮
				else if(j == 21 && aName[0] != 'G')		//高级软卧
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aGaoRuanWo);
				}
				else if(j == 23 && aName[0] != 'G')		//一等软卧
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aYiRuanWo);
				}
				else if(j == 26 && aName[0] != 'G')		//无座
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aWuZuo);
				}
				else if(j == 24 )		//软坐
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aRuanZuo);
				}
				else if(j == 29 )		//硬坐
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aYingZuo);
				}
				else if(j == 28 )		//硬卧
				{
					sscanf(&aTempBuff[u32Index],"%[^|]",aYingWo);
				}


				while(u32Index < u32Len && aTempBuff[u32Index] != '|')u32Index++;
				u32Index++;

			}

			STATION_MAP_Get_1_Name(aStart);
			STATION_MAP_Get_1_Name(aEnd);
			STATION_MAP_Get_1_Name(aFrom);
			STATION_MAP_Get_1_Name(aTo);
			printf("|%3d |\033[33m%5s\033[0m |",i,aName);
			MAIN_Print_Data(aStart);
			MAIN_Print_Data(aEnd);
			MAIN_Print_Data(aFrom);
			MAIN_Print_Data(aTo);
			
			printf("   \033[34m%5s\033[0m  |   \033[36m%5s\033[0m  |  \033[32m%5s\033[0m  |",aStartTime,aEndTime,aAllTime);

			//特等
			if(strcmp(aTeDeng,"有") == 0 )
			{
				printf("特等:\033[32m%4s\033[0m|",aTeDeng);
			}
			else if(strcmp(aTeDeng,"无") == 0)
			{
				printf("特等:\033[31m%4s\033[0m|",aTeDeng);
			}
			else if(strlen(aTeDeng) == 0)
			{
				/** printf("特等: - |"); */
			}
			else
			{
				printf("特等:\033[33m%3s\033[0m|",aTeDeng);
			}

			//一等
			if(strcmp(aYiDeng,"有") == 0 )
			{
				printf("一等:\033[32m%4s\033[0m|",aYiDeng);
			}
			else if(strcmp(aYiDeng,"无") == 0)
			{
				printf("一等:\033[31m%4s\033[0m|",aYiDeng);
			}
			else if(strlen(aYiDeng) == 0)
			{
				/** printf("一等: - |"); */
			}
			else
			{
				printf("一等:\033[33m%3s\033[0m|",aYiDeng);
			}

			//二等
			if(strcmp(aErDeng,"有") == 0)
			{
				printf("二等:\033[32m%4s\033[0m|",aErDeng);
			}
			else if(strcmp(aErDeng,"无") == 0)
			{
				printf("二等:\033[31m%4s\033[0m|",aErDeng);
			}
			else if(strlen(aErDeng) == 0)
			{
				/** printf("二等: - |"); */
			}
			else
			{
				printf("二等:\033[33m%3s\033[0m|",aErDeng);
			}


			//高卧
			if(strcmp(aGaoRuanWo ,"有") == 0)
			{
				printf("高卧:\033[32m%4s\033[0m|",aGaoRuanWo);
			}
			else if(strcmp(aGaoRuanWo ,"无") == 0)
			{
				printf("高卧:\033[31m%4s\033[0m|",aGaoRuanWo);
			}
			else if(strlen(aGaoRuanWo) == 0)
			{
				/** printf("高卧: - |"); */
			}
			else
			{
				printf("高卧:\033[33m%3s\033[0m|",aGaoRuanWo);
			}

			//一卧
			if(strcmp(aYiRuanWo ,"有") == 0)
			{
				printf("一卧:\033[32m%4s\033[0m|",aYiRuanWo);
			}
			else if(strcmp(aYiRuanWo ,"无") == 0)
			{
				printf("一卧:\033[31m%4s\033[0m|",aYiRuanWo);
			}
			else if(strlen(aYiRuanWo) == 0)
			{
				/** printf("一卧: - |"); */
			}
			else
			{
				printf("一卧:\033[33m%3s\033[0m|",aYiRuanWo);
			}

			//动卧
			if(strcmp(aDongWo ,"有") == 0)
			{
				printf("动卧:\033[32m%4s\033[0m|",aDongWo);
			}
			else if(strcmp(aDongWo ,"无") == 0)
			{
				printf("动卧:\033[31m%4s\033[0m|",aDongWo);
			}
			else if(strlen(aDongWo) == 0)
			{
				/** printf("动卧: - |"); */
			}
			else
			{
				printf("动卧:\033[33m%3s\033[0m|",aDongWo);
			}

			//硬卧
			if(strcmp(aYingWo ,"有") == 0)
			{
				printf("硬卧:\033[32m%4s\033[0m|",aYingWo);
			}
			else if(strcmp(aYingWo ,"无") == 0)
			{
				printf("硬卧:\033[31m%4s\033[0m|",aYingWo);
			}
			else if(strlen(aYingWo) == 0)
			{
				/** printf("硬卧: - |"); */
			}
			else
			{
				printf("硬卧:\033[33m%3s\033[0m|",aYingWo);
			}

			//软座
			if(strcmp(aRuanZuo ,"有") == 0)
			{
				printf("软座:\033[32m%4s\033[0m|",aRuanZuo);
			}
			else if(strcmp(aRuanZuo ,"无") == 0)
			{
				printf("软座:\033[31m%4s\033[0m|",aRuanZuo);
			}
			else if(strlen(aRuanZuo) == 0)
			{
				/** printf("软座: - |"); */
			}
			else
			{
				printf("软座:\033[33m%3s\033[0m|",aRuanZuo);
			}

			//硬座
			if(strcmp(aYingZuo ,"有") == 0)
			{
				printf("硬座:\033[32m%4s\033[0m|",aYingZuo);
			}
			else if(strcmp(aYingZuo ,"无") == 0)
			{
				printf("硬座:\033[31m%4s\033[0m|",aYingZuo);
			}
			else if(strlen(aYingZuo) == 0)
			{
				/** printf("硬座: - |"); */
			}
			else
			{
				printf("硬座:\033[33m%3s\033[0m|",aYingZuo);
			}

			//无座
			if(strcmp(aWuZuo ,"有") == 0)
			{
				printf("无座:\033[32m%4s\033[0m|",aWuZuo);
			}
			else if(strcmp(aWuZuo ,"无") == 0)
			{
				printf("无座:\033[31m%4s\033[0m|",aWuZuo);
			}
			else if(strlen(aWuZuo) == 0)
			{
				/** printf("无座: - |"); */
			}
			else
			{
				printf("无座:\033[33m%3s\033[0m|",aWuZuo);
			}
			
			printf("\n");

			/** printf("特等:%3s|一等:%3s|二等:%3s|高级软卧:%3s|一等软卧:%3s|动卧:%3s|软座:%3s|硬座:%3s|无座:%3s|\n",aTeDeng,aYiDeng,aErDeng,aGaoRuanWo,aYiRuanWo,aDongWo,aRuanZuo,aYingZuo,aWuZuo); */


			memset(aTempBuff,0,MAX_DATA_SIZE);
		}
	}


	printf("+---------------------------------------------------------------------------------------------------------------------------+\n");
	cJSON_Delete(cjRoot);
	return 0;

}


/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int main(int argc, char **argv)
{

	if(argc != 4)
	{
		printf("\033[31m参数数量:%d有误！\033[0m\n",argc);
		printf("\n  \033[32m格式:%s 时间 始发站 终点站\033[0m\n",argv[0]);
		printf("\n  \033[32m时间		:07-20\033[0m\n");
		printf("  \033[32m始发站	:广州\033[0m\n");
		printf("  \033[32m终点站	:深圳\033[0m\n");
		printf("\n  \033[32m例如:%s 07-20 广州 深圳\033[0m\n",argv[0]);
		return 0;
	}

	/**** 判断参数 ****/

	char aDay[30];

	//时间
	time_t u32Time;	
	time(&u32Time);
	struct tm *stTime;
	stTime = localtime(&u32Time);
	//自加的原因是 函数计算出来的数据 是减了1的
	stTime->tm_mon++;
	
	int32_t s32Mon = 0 , s32Day = 0;
	sscanf(argv[1],"%d-%d",&s32Mon,&s32Day);
	//判断时间有误
	if( s32Mon == 0 || s32Day == 0 || (s32Mon < stTime->tm_mon) || (s32Day < stTime->tm_mday && s32Mon <= stTime->tm_mon ))
	{
		printf("\033[31m时间有误，请输入今天的日期或半个月以内的日期\033[0m\n");
		return 0;
	}
	//时间正确
	memset(aDay,0,sizeof(aDay));
	sprintf(aDay,"%d-%02d-%02d",stTime->tm_year+1900,s32Mon,s32Day);

	//始发站
	/** uint8_t u8FromSize = 0; */
	char aFromCode[10];
	strcpy(aFromCode,argv[2]);


	/** char **ppcFromCode = NULL; */
	//查找车站表
	/** int ret = STATION_MAP_Get_Code(&ppcFromCode, argv[2], &u8FromSize); */
	int ret = STATION_MAP_Get_1_Code(aFromCode);
	if(ret != 0)
	{
		printf("\033[31m程序出错\033[0m\n");
		return 0;
	}

	/** if(u8FromSize == 0) */
	/** { */
	/**     printf("\033[31m查找不到始发站%s的车站信息\033[0m\n",argv[2]); */
	/**     return 0; */
	/** } */

	/** for ( uint8_t i = 0 ; i < u8FromSize ; i++ ) */
	/** { */
		/** printf("[%d]%s\n",i,*(ppcFromCode+i)); */
	/** } */
	

	//终点站
	/** uint8_t u8ToSize = 0; */
	char aToCode[10];
	strcpy(aToCode,argv[3]);
	/** char **ppcToCode = NULL; */
	//查找车站表
	/** ret = STATION_MAP_Get_Code(&ppcToCode, argv[3], &u8ToSize); */
	ret = STATION_MAP_Get_1_Code(aToCode);
	if(ret != 0)
	{
		printf("\033[31m程序出错\033[0m\n");
		return 0;
	}

	/** if(u8ToSize == 0) */
	/** { */
	/**     printf("\033[31m查找不到终点站%s的车站信息\033[0m\n",argv[3]); */
	/**     return 0; */
	/** } */

	/** for ( uint8_t i = 0 ; i < u8ToSize ; i++ ) */
	/** { */
		/** printf("[%d]%s\n",i,*(ppcToCode+i)); */
	/** } */


	char *pcData = malloc(MAX_DATA_SIZE);
	/** ret = TRAIN_HTTPS_Get_Quest_Data(pcData,aDay,ppcFromCode[0],ppcToCode[0]); */
	ret = TRAIN_HTTPS_Get_Quest_Data(pcData,aDay,aFromCode,aToCode);
	if(ret != 0)
	{
		printf("\033[31m获取到12306过程出错\033[0m\n");
	}
	/** printf("%s\n",pcData); */

	MAIN_Parse_Train_JSON(pcData);



	/**** 释放空间 ****/
	/** for ( uint8_t i = 0; i < u8FromSize ; i ++ ) */
	/** { */
	/**     free(*(ppcFromCode+i)); */
	/** } */
	/** free(ppcFromCode); */

	/** for ( uint8_t i = 0; i < u8ToSize ; i ++ ) */
	/** { */
	/**     free(*(ppcToCode+i)); */
	/** } */
	/** free(ppcToCode); */
	free(pcData);


}







