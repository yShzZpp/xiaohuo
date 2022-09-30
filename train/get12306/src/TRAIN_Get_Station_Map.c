/************************************************************************
 * * @Author  :YaoShiHong
 * * @Company :
 * * @Date    :2022/07/19  
 * * @file    :
 * ************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TRAIN_Get_Station_Map.h"

/***********************************常量***********************************/
#define STATION_MAP_PATH "get12306/12306StationMap"

/***********************************函数定义***********************************/

typedef struct train_info_cache_st
{
	char aName[30];
	char aCode[10];
}TRAIN_INFO_ST;

typedef struct train_cache_st
{
	uint32_t u32Size;
	TRAIN_INFO_ST stInfo[0];

}TRAIN_CACHE_ST;

TRAIN_CACHE_ST *g_pstCache = NULL;

/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int8_t STATION_MAP_Get_1_Code(char *pcName)
{
	char aCmd[100],aBuff[1024],aName[30],aCode[10];
	uint32_t u32Size = 0,u32Index = 0;

	//查看本地是否有保存
	if(g_pstCache != NULL)
	{
		for(uint32_t i = 0 ; i < g_pstCache->u32Size; i++)
		{
			if(strcmp(pcName,g_pstCache->stInfo[i].aName) == 0)
			{
				strcpy(pcName,g_pstCache->stInfo[i].aCode);
				return 0;
			}
		}
	}

	strcpy(aName,pcName);

	//查询数量	
	uint8_t u8Num = 0;
	sprintf(aCmd,"grep \"%s\" %s|wc -l",pcName,STATION_MAP_PATH);
	
	//查询
	FILE *fp = popen(aCmd, "r");
	memset(aBuff,0,sizeof(aBuff));
	u32Size = fread(aBuff, 1, 20, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	//获取数量
	u8Num = atoi(aBuff);
	fclose(fp);

	if(u8Num == 0)
	{
		printf("找不到%s对应的名称\n",pcName);
		return -1;
	}

	sprintf(aCmd, "grep \"%s\" %s",pcName,STATION_MAP_PATH);

	fp = popen(aCmd, "r");
	u32Size = fread(aBuff, 1, 1024, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	memset(pcName,0,strlen(pcName));
	sscanf(aBuff,"%*[^;];%s",pcName);
	strcpy(aCode,pcName);


	//开辟空间
	if(g_pstCache == NULL)
	{
		g_pstCache = malloc(sizeof(TRAIN_INFO_ST)+ sizeof(uint32_t));
		memset(g_pstCache,0,sizeof(TRAIN_INFO_ST)+ sizeof(uint32_t));
	}
	//扩展空间
	else
	{
		g_pstCache = realloc(g_pstCache, sizeof(uint32_t) + sizeof(TRAIN_INFO_ST) * ( g_pstCache->u32Size + 1 ));
		memset(&g_pstCache->stInfo[g_pstCache->u32Size],0,sizeof(TRAIN_INFO_ST));
	}


	strcpy(g_pstCache->stInfo[g_pstCache->u32Size].aCode,aCode);
	strcpy(g_pstCache->stInfo[g_pstCache->u32Size].aName,aName);
	g_pstCache->u32Size++;

	fclose(fp);
	return 0;
}

/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int8_t STATION_MAP_Get_1_Name(char *pcCode)
{
	char aCmd[100],aBuff[1024],aCode[10],aName[30];
	uint32_t u32Size = 0,u32Index = 0;

	//查看本地是否有保存
	if(g_pstCache != NULL)
	{
		for(uint32_t i = 0 ; i < g_pstCache->u32Size; i++)
		{
			if(strcmp(pcCode,g_pstCache->stInfo[i].aCode) == 0)
			{
				strcpy(pcCode,g_pstCache->stInfo[i].aName);
				return 0;
			}
		}

	}

	strcpy(aCode,pcCode);

	//查询数量	
	uint8_t u8Num = 0;
	sprintf(aCmd,"grep \"%s\" %s|wc -l",pcCode,STATION_MAP_PATH);
	
	//查询
	FILE *fp = popen(aCmd, "r");
	memset(aBuff,0,sizeof(aBuff));
	u32Size = fread(aBuff, 1, 20, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	//获取数量
	u8Num = atoi(aBuff);
	fclose(fp);

	if(u8Num == 0)
	{
		printf("找不到%s对应的名称\n",pcCode);
		return -1;
	}

	sprintf(aCmd, "grep \"%s\" %s",pcCode,STATION_MAP_PATH);

	fp = popen(aCmd, "r");
	u32Size = fread(aBuff, 1, 1024, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	memset(pcCode,0,strlen(pcCode));
	sscanf(aBuff,"%[^;]",pcCode);
	strcpy(aName,pcCode);





	//开辟空间
	if(g_pstCache == NULL)
	{
		g_pstCache = malloc(sizeof(TRAIN_INFO_ST)+ sizeof(uint32_t));
		memset(g_pstCache,0,sizeof(TRAIN_INFO_ST)+ sizeof(uint32_t));
	}
	//扩展空间
	else
	{
		g_pstCache = realloc(g_pstCache, sizeof(uint32_t) + sizeof(TRAIN_INFO_ST) * ( g_pstCache->u32Size + 1 ));
		memset(&g_pstCache->stInfo[g_pstCache->u32Size],0,sizeof(TRAIN_INFO_ST));
	}


	strcpy(g_pstCache->stInfo[g_pstCache->u32Size].aCode,aCode);
	strcpy(g_pstCache->stInfo[g_pstCache->u32Size].aName,aName);
	g_pstCache->u32Size++;



	fclose(fp);
	return 0;
}

/******************************************************
 * ****** Function		:   STATION_MAP_Get_Name
 * ****** brief			:	通过代号查询站台名称
 * ****** param			:	@pcCode:需要查询的代码
 *							@pppcName:存放查询到的名称
 * ****** return		:	成功找到返回0，找不到返回-1
 * *******************************************************/
int8_t STATION_MAP_Get_Name(char *pcCode,char ***pppcName,uint8_t *pu8Size)
{
	char aCmd[100],aBuff[1024];
	uint32_t u32Size = 0,u32Index = 0;


	//查询数量	
	uint8_t u8Num = 0;
	sprintf(aCmd,"grep \"%s\" %s|wc -l",pcCode,STATION_MAP_PATH);
	
	//查询
	FILE *fp = popen(aCmd, "r");
	memset(aBuff,0,sizeof(aBuff));
	u32Size = fread(aBuff, 1, 20, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	//获取数量
	u8Num = atoi(aBuff);
	*pu8Size = u8Num;
	/** printf("获取名称 有%d个数据\n",u8Num); */
	fclose(fp);

	//数量为0
	if(u8Num == 0)
	{
		return 0;
	}

	//开辟空间  创建数量大小的二级char指针 每个指针指向一个名字
	*pppcName = (char **)malloc(u8Num * sizeof(char **));
	memset(*pppcName,0,u8Num * sizeof(char **));

	//获取数据
	memset(aBuff, 0, sizeof(aBuff));
	memset(aCmd, 0, sizeof(aCmd));
	sprintf(aCmd, "grep \"%s\" %s",pcCode,STATION_MAP_PATH);

	fp = popen(aCmd, "r");
	u32Size = fread(aBuff, 1, 1024, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	fclose(fp);

	char aName[30];
	//获取每一个的数量
	for(uint8_t i = 0 ; i < u8Num ; i++)
	{
		memset(aName,0,sizeof(aName));
		sscanf(&aBuff[u32Index++],"%[^;]",aName);
		while(u32Index < u32Size && aBuff[u32Index == 0 ? 0:(u32Index -1)] != '\n')
		{
			u32Index++;
		}
		//**pppcName 指向第一个名称 *(*pppcName + 1)指向第二个名称 以此类推 
		//开辟名称的空间
		*(*pppcName+i) = (char *)malloc(30);
		//清空
		memset(*(*pppcName+i),0,30);
		//赋值
		memcpy(*(*pppcName+i),aName,strlen(aName));
	}

	return 0;
}

/******************************************************
 * ****** Function		:   STATION_MAP_Get_Code
 * ****** brief			:	通过名称查询站台代码
 * ****** param			:	@**pppcCode:存放查询到的代码
 *							@pcName:需要查询的名称
 * ****** return		:	成功找到返回0，找不到返回-1
 * *******************************************************/
int8_t STATION_MAP_Get_Code(char ***pppcCode,char *pcName,uint8_t *pu8Size)
{

	char aCmd[100],aBuff[1024];
	uint32_t u32Size = 0,u32Index = 0;


	//查询数量	
	uint8_t u8Num = 0;
	sprintf(aCmd,"grep \"%s\" %s|wc -l",pcName,STATION_MAP_PATH);
	
	//查询
	FILE *fp = popen(aCmd, "r");
	memset(aBuff,0,sizeof(aBuff));
	u32Size = fread(aBuff, 1, 20, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	//获取数量
	u8Num = atoi(aBuff);
	*pu8Size = u8Num;
	/** printf("获取代码 有%d个数据\n",u8Num); */
	fclose(fp);

	//数量为0
	if(u8Num == 0)
	{
		return 0;
	}

	//开辟空间  创建数量大小的二级char指针 每个指针指向一个代码
	*pppcCode = (char **)malloc(u8Num * sizeof(char **));
	memset(*pppcCode,0,u8Num * sizeof(char **));

	//获取数据
	memset(aBuff, 0, sizeof(aBuff));
	memset(aCmd, 0, sizeof(aCmd));
	sprintf(aCmd, "grep \"%s\" %s",pcName,STATION_MAP_PATH);

	fp = popen(aCmd, "r");
	u32Size = fread(aBuff, 1, 1024, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	fclose(fp);

	char aCode[10];
	//获取每一个的数量
	for(uint8_t i = 0 ; i < u8Num ; i++)
	{
		memset(aCode,0,sizeof(aCode));
		sscanf(&aBuff[u32Index++],"%*[^;];%s",aCode);
		while(u32Index < u32Size && aBuff[u32Index == 0 ? 0:(u32Index -1)] != '\n')
		{
			u32Index++;
		}
		//**pppcCode 指向第一个代码 *(*pppcCode + 1)指向第二个代码 以此类推 
		//开辟代码的空间
		*(*pppcCode+i) = (char *)malloc(10);
		//清空
		memset(*(*pppcCode+i),0,10);
		//赋值
		memcpy(*(*pppcCode+i),aCode,strlen(aCode));
	}

	return 0;
}
