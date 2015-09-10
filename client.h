
/***********************************************
************************************************
该文件为客户机通用接口函数头文件，基于linux下TCP/IP协议
作者：Splendor.G
时间：2015年9月9日15:05:18
************************************************
************************************************/


#ifndef		__CLIENT_H__
#define		__CLIENT_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

//端口号
#define     PORT    8000

int udp_client_uici();

#endif
