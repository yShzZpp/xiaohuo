#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

#define MAX_TRAIN_SIZE 20
#define FILE_PATH "./train_info.txt"

struct train_info
{
	char cName[10];
	char cFrom[16];
	char cDest[16];
	u_int32_t u32Tickets;
	char cStartTime[20];
	char cEndTime[20];
};

/******************************************************
 * ****** Function		:   put_all_trains_info
 * ****** brief			:	打印保存的所有车票信息
 * ****** param			:	@pstTrain:结构体指针
 *							@u32Size:结构体指针指向的结构体数量
 * ****** return		:	成功返回0，错误返回-1
 * *******************************************************/
int8_t put_all_trains_info( struct train_info *pstTrain ,u_int32_t u32Size)
{
	if(u32Size == 0)
	{
		printf("	无信息,请先添加信息\n");
		return -1;
	}

	if(pstTrain == NULL)
	{
		printf("	传入结构体指针错误\n");
		return -1;
	}
	
	printf("					车次信息\n");
	for ( int i = 0 ; i < u32Size ; i ++ )
	{
		printf("	[%d]%s-->%s 车次:%s 剩余车票:%d 发车时间:%s 抵达时间:%s\n",i,pstTrain[i].cFrom,pstTrain[i].cDest,pstTrain[i].cName,pstTrain[i].u32Tickets,pstTrain[i].cStartTime,pstTrain[i].cEndTime);
	}
	return 0;
}

/******************************************************
 * ****** Function		:   put_one_train_info
 * ****** brief			:	打印指定结构体的车辆信息
 * ****** param			:	@stTrain:需要打印的信息
 * ****** return		:   NULL
 * *******************************************************/
void put_one_train_info(struct train_info stTrain)
{
	printf("	%s-->%s 车次:%s 剩余车票:%d 发车时间:%s 抵达时间:%s\n",stTrain.cFrom,stTrain.cDest,stTrain.cName,stTrain.u32Tickets,stTrain.cStartTime,stTrain.cEndTime);
}

/******************************************************
 * ****** Function		:   find_train_by_name
 * ****** brief			:	通过车次编号找到对应的数据
 * ****** param			:	@pstTrain:存放所有数据的结构体指针
 *							@u32Size:结构体的数量
 *							@pcTarget:目标车次名称
 * ****** return		:	成功返回0，错误返回-1
 * *******************************************************/
int8_t find_train_by_name(struct train_info *pstTrain,uint32_t u32Size,char *pcTarget)
{
	bool bFind = false;
	int8_t s8Ret = -1;


	if(u32Size == 0)
	{
		printf("	无信息,请先添加信息\n");
		return -1;
	}

	if(pstTrain == NULL)
	{
		printf("	传入结构体指针错误\n");
		return -1;
	}


	for ( int i = 0 ; i < u32Size ; i ++ )
	{
		//找到对应的名称
		if(strcmp(pcTarget,pstTrain[i].cName) == 0)
		{
			put_one_train_info(pstTrain[i]);	
			bFind = true;
			s8Ret = i;
		}
	}

	if(!bFind)
	{
		printf("	找不到对应信息\n");
	}

	return s8Ret;
	
}

/******************************************************
 * ****** Function		:   write_train_info_to_file
 * ****** brief			:	将车辆信息写入文件
 * ****** param			:	@pstTrain:存放所有数据的结构体指针
 *							@u32Size:结构体的数量
 * ****** return		:   NULL
 * *******************************************************/
int8_t write_train_info_to_file(struct train_info *pstTrain,uint32_t u32Size)
{
	if(u32Size == 0)
	{
		printf("	无信息,请先添加信息\n");
		return -1;
	}

	if(pstTrain == NULL)
	{
		printf("	传入结构体指针错误\n");
		return -1;
	}


	//有文件 w+:则清空并写入，无文件则创建
	FILE *fp = fopen(FILE_PATH,"w+");
	
	char buff[1024];

	for( uint32_t i = 0 ; i < u32Size ; i++ )
	{
		memset(buff,0,1024);
		//按这种格式写入 方便后续读取 末尾的换行符用于标识一行数据的结尾
		sprintf(buff,"from:%s;dest:%s;name:%s;ticket:%d;start:%s;end:%s\n",
					pstTrain[i].cFrom,pstTrain[i].cDest,pstTrain[i].cName,
					pstTrain[i].u32Tickets,pstTrain[i].cStartTime,pstTrain[i].cEndTime);
		fwrite(buff, strlen(buff), 1, fp);
	}

	fclose(fp);
	return 0;
}

/******************************************************
 * ****** Function		:   read_train_info_from_file
 * ****** brief			:	将车辆信息写入文件
 * ****** param			:	@pstTrain:存放所有数据的结构体指针
 *							@u32Size:返回数据的数量
 * ****** return		:   NULL
 * *******************************************************/
int8_t read_train_info_from_file(struct train_info *pstTrain,uint32_t *pu32Size)
{
	if(pstTrain == NULL)
	{
		printf("	传入结构体指针错误\n");
		return -1;
	}

	FILE *fp = fopen(FILE_PATH,"r");

	if(fp == NULL)
	{
		printf("文件为空 暂无保存的信息\n");
		return 0;
	}
	
	char cAllBuff[10240];
	//读取文件 并且获取文件数据长度
	int s32Len = fread(cAllBuff, 1,10240, fp);
	
	//用于标识每一行的开头相对于数据的偏移量
	int s32Index = 0;

	uint32_t u32Size = *pu32Size;

	while(s32Index < s32Len && u32Size < MAX_TRAIN_SIZE)
	{
		//看不懂的话 这个man手册
		//https://man7.org/linux/man-pages/man3/scanf.3.html
		//或者csdn
		//https://blog.csdn.net/qq_37182906/article/details/108317046
		sscanf(&cAllBuff[s32Index],"from:%[^;];dest:%[^;];name:%[^;];ticket:%d;start:%[^;];end:%s",
			  pstTrain[u32Size].cFrom,pstTrain[u32Size].cDest,
			  pstTrain[u32Size].cName,&pstTrain[u32Size].u32Tickets,
			  pstTrain[u32Size].cStartTime,pstTrain[u32Size].cEndTime);
		//找到每一行的数据结尾 结尾就是换行符 写入文件的时候写入的换行符号
		while(s32Index < s32Len && cAllBuff[s32Index] != '\n')
		{
			s32Index++;
		}
		s32Index++;
		u32Size++;
	}

	*pu32Size = u32Size;

	fclose(fp);
	return 0;
}



int main(int argc,char *argv[])
{
	int choice;
	uint32_t u32Size = 0;
	struct train_info astTrain[MAX_TRAIN_SIZE];
	memset(astTrain, 0, sizeof(astTrain));
	char cName[10];
	char cFrom[20];
	char cDest[20];
	uint32_t u32Tickets;
	char cStartTime[20];
	char cEndTime[20];

	read_train_info_from_file(astTrain, &u32Size);
	while(1)
	{
		printf("\n+---------------------------------------------------------------------------------------------------+\n");
		printf("欢迎\n1:查看全部\n2:通过名称查找车次\n3:添加车次信息\n0:退出\n");
		scanf("%d",&choice);

		switch(choice)
		{
			case 1:
				printf("|---------------------------------------------------------------------------------------------------|\n");
				put_all_trains_info(astTrain,u32Size);
			break;

			case 2:
				printf("|---------------------------------------------------------------------------------------------------|\n");
				printf("	请输入需要查找的名称\n");
				scanf("%s",cName);
				find_train_by_name(astTrain, u32Size,cName );
				
			break;

			case 3:

				if(u32Size >= MAX_TRAIN_SIZE)
				{
					printf("	存储的信息已满\n");
					break;
				}
				
				printf("|---------------------------------------------------------------------------------------------------|\n");
				printf("	请输入始发地(str) 目的地(str) 车次名称(str) 剩余车票(int) 发车时间(str) 抵达时间(str)\n");
				scanf("%s %s %s %d %s %s",astTrain[u32Size].cFrom,astTrain[u32Size].cDest,
										astTrain[u32Size].cName,&astTrain[u32Size].u32Tickets,
										astTrain[u32Size].cStartTime,astTrain[u32Size].cEndTime);
				u32Size++;
				write_train_info_to_file(astTrain, u32Size);
					
			break;

			case 0:
				exit(0);

			default:
				printf("无效输入\n");
				
		}
		printf("+---------------------------------------------------------------------------------------------------+\n");
	}
	return 0;
}
