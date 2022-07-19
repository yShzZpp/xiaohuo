#ifndef __TRAIN_HTTPS_H
#define __TRAIN_HTTPS_H

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
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define MAX_DATA_SIZE (1024*1024)




int TRAIN_HTTPS_Get_Quest_Data(char *pcData,char *pcTime,char *pcFrom,char *pcTo);
#endif
