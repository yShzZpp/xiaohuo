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

/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int Parse_Train_data(char *pcData)
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

	//获取所需信息
	uint32_t u32ResultSize = cJSON_GetArraySize(cjResultArray);
	char aName[10],aStart[30],aEnd[30],aFrom[30],aTo[30],aStartTime[20],aEndTime[20],aAllTime[20];
	printf("有%d条车辆信息\n",u32ResultSize);
	for(uint32_t i = 0 ; i < u32ResultSize ; i++)
	{
		cJSON *cjTemp = cJSON_GetArrayItem(cjResultArray, i);
		if(cjTemp != NULL && cjTemp->type == cJSON_String)
		{
			//							跳过	跳过		跳过	名称	
			sscanf(cjTemp->valuestring,"%*[^|] | %*[^|] | %*[^|] | %9[^|] | "
					//始发站	终点站  来站	去站
					" %9[^|] | %9[^|] | %9[^|] | %9[^|] |"
					//发车时间	抵达时间 总时间
					" %9[^|] | %9[^|] | %9[^|] |"
					,aName,aStart,aEnd,aFrom,aTo,
					aStartTime,aEndTime,aAllTime);

			STATION_MAP_Get_1_NAME(aStart);
			STATION_MAP_Get_1_NAME(aEnd);
			STATION_MAP_Get_1_NAME(aFrom);
			STATION_MAP_Get_1_NAME(aTo);
			printf("[%d]车次:\033[32m%s\033[0m 始发站:\033[32m%s\033[0m 终点站:\033[32m%s\033[0m\n",i,aName,aStart,aEnd);
			printf("   查询始发站:\033[32m%s\033[0m 查询终点站:\033[32m%s\033[0m\n",aFrom,aTo);
			printf("   发车时间:\033[32m%s\033[0m 抵达时间:\033[32m%s\033[0m 总时间:\033[32m%s\033[0m\n",aStartTime,aEndTime,aAllTime);
		}
	}


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
	uint8_t u8FromSize = 0;

	char **ppcFromCode = NULL;
	//查找车站表
	int ret = STATION_MAP_Get_Code(&ppcFromCode, argv[2], &u8FromSize);
	if(ret != 0)
	{
		printf("\033[31m程序出错\033[0m\n");
		return 0;
	}

	if(u8FromSize == 0)
	{
		printf("\033[31m查找不到始发站%s的车站信息\033[0m\n",argv[2]);
		return 0;
	}

	for ( uint8_t i = 0 ; i < u8FromSize ; i++ )
	{
		printf("[%d]%s\n",i,*(ppcFromCode+i));
	}
	

	//终点站
	uint8_t u8ToSize = 0;

	char **ppcToCode = NULL;
	//查找车站表
	ret = STATION_MAP_Get_Code(&ppcToCode, argv[3], &u8ToSize);
	if(ret != 0)
	{
		printf("\033[31m程序出错\033[0m\n");
		return 0;
	}

	if(u8ToSize == 0)
	{
		printf("\033[31m查找不到终点站%s的车站信息\033[0m\n",argv[3]);
		return 0;
	}

	for ( uint8_t i = 0 ; i < u8ToSize ; i++ )
	{
		printf("[%d]%s\n",i,*(ppcToCode+i));
	}


	char *pcData = malloc(MAX_DATA_SIZE);
	ret = TRAIN_HTTPS_Get_Quest_Data(pcData,aDay,ppcFromCode[0],ppcToCode[0]);
	if(ret != 0)
	{
		printf("\033[31m获取到12306过程出错\033[0m\n");
	}
	/** printf("%s\n",pcData); */

	Parse_Train_data(pcData);



	/**** 释放空间 ****/
	for ( uint8_t i = 0; i < u8FromSize ; i ++ )
	{
		free(*(ppcFromCode+i));
	}
	free(ppcFromCode);

	for ( uint8_t i = 0; i < u8ToSize ; i ++ )
	{
		free(*(ppcToCode+i));
	}
	free(ppcToCode);
	free(pcData);


}







