/************************************************************************
 * * @Author  :YaoShiHong
 * * @Company :
 * * @Date    :2022/07/19  
 * * @file    :
 * ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/TRAIN_Get_Station_Map.h"

/***********************************常量***********************************/
#define STATION_MAP_PATH "get12306/12306StationMap"

/***********************************函数定义***********************************/

/******************************************************
 * ****** Function		:   
 * ****** brief			:
 * ****** param			:   NULL
 * ****** return		:   NULL
 * *******************************************************/
int8_t STATION_MAP_Get_1_NAME(char *pcCode)
{
	char cCmd[100],cBuff[1024];
	uint32_t u32Size = 0,u32Index = 0;

	//查询数量	
	uint8_t u8Num = 0;
	sprintf(cCmd,"grep \"%s\" %s|wc -l",pcCode,STATION_MAP_PATH);
	
	//查询
	FILE *fp = popen(cCmd, "r");
	memset(cBuff,0,sizeof(cBuff));
	u32Size = fread(cBuff, 1, 20, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	//获取数量
	u8Num = atoi(cBuff);
	fclose(fp);

	if(u8Num == 0)
	{
		printf("找不到%s对应的名称\n",pcCode);
		return -1;
	}

	sprintf(cCmd, "grep \"%s\" %s",pcCode,STATION_MAP_PATH);

	fp = popen(cCmd, "r");
	u32Size = fread(cBuff, 1, 1024, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	memset(pcCode,0,strlen(pcCode));
	sscanf(cBuff,"%[^;]",pcCode);

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
	char cCmd[100],cBuff[1024];
	uint32_t u32Size = 0,u32Index = 0;


	//查询数量	
	uint8_t u8Num = 0;
	sprintf(cCmd,"grep \"%s\" %s|wc -l",pcCode,STATION_MAP_PATH);
	
	//查询
	FILE *fp = popen(cCmd, "r");
	memset(cBuff,0,sizeof(cBuff));
	u32Size = fread(cBuff, 1, 20, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	//获取数量
	u8Num = atoi(cBuff);
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
	memset(cBuff, 0, sizeof(cBuff));
	memset(cCmd, 0, sizeof(cCmd));
	sprintf(cCmd, "grep \"%s\" %s",pcCode,STATION_MAP_PATH);

	fp = popen(cCmd, "r");
	u32Size = fread(cBuff, 1, 1024, fp);
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
		sscanf(&cBuff[u32Index++],"%[^;]",aName);
		while(u32Index < u32Size && cBuff[u32Index == 0 ? 0:(u32Index -1)] != '\n')
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

	char cCmd[100],cBuff[1024];
	uint32_t u32Size = 0,u32Index = 0;


	//查询数量	
	uint8_t u8Num = 0;
	sprintf(cCmd,"grep \"%s\" %s|wc -l",pcName,STATION_MAP_PATH);
	
	//查询
	FILE *fp = popen(cCmd, "r");
	memset(cBuff,0,sizeof(cBuff));
	u32Size = fread(cBuff, 1, 20, fp);
	if(u32Size == 0 )
	{
		printf("读取错误\n");
		return -1;
	}

	//获取数量
	u8Num = atoi(cBuff);
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
	memset(cBuff, 0, sizeof(cBuff));
	memset(cCmd, 0, sizeof(cCmd));
	sprintf(cCmd, "grep \"%s\" %s",pcName,STATION_MAP_PATH);

	fp = popen(cCmd, "r");
	u32Size = fread(cBuff, 1, 1024, fp);
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
		sscanf(&cBuff[u32Index++],"%*[^;];%s",aCode);
		while(u32Index < u32Size && cBuff[u32Index == 0 ? 0:(u32Index -1)] != '\n')
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
