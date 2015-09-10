
/***********************************************
************************************************
该文件为客户机通用接口函数源，基于linux下TCP/IP协议
作者：Splendor.G
时间：2015年9月9日15:05:18
************************************************
************************************************/

#include "client.h"

//UDP通用socket头，函数成功返回socket描述符，错误结束程序
int udp_client_uici()
{
	const int val = 1;
	volatile int cli_sock = 0;

	//创建套接字
	cli_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(-1 == cli_sock)
	{
		perror("UDP:client socket failed");
		exit(-1);
	}

	//设置服务器端口可重用
	if(-1 == setsockopt(cli_sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
	{
		perror("setsockopt client failed");
		if(cli_sock != -1)
		{
			close(cli_sock);
		}
		exit(-1);
	}

	return cli_sock;
}
