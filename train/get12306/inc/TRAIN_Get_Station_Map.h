#ifndef __TRAIN_GET_STATION_MAP_H__
#define __TRAIN_GET_STATION_MAP_H__

/************************************************************************
 * * @Author  :YaoShiHong
 * * @Company :
 * * @Date    :2022/07/19  
 * * @file    :站台信息查询头文件
 * ************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*************************************结构体*********************************/

typedef struct station_map_st
{
	const char cCode[10];
	const char cName[30];
}STATION_MAP_ST;


/***********************************函数声明***********************************/

int8_t STATION_MAP_Get_1_NAME(char *pcCode);
int8_t STATION_MAP_Get_Name(char *pcCode,char ***pppcName,uint8_t *pu8Size);
int8_t STATION_MAP_Get_Code(char ***pppcCode,char *pcName,uint8_t *pu8Size);


#endif
