
#ifndef		__SERVER_H__
#define		__SERVER_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

//端口号
#define		PORT	8000

//注意：此函数只适用于Liunx系统中TCP协议的IPV4模式
//参数max_cli表示服务器所能接受的最大客户量
//参数ser_port为服务器端口号
//参数ser_ip为服务器ip地址,如果为NULL，则由系统自动分配ip
//本函数成功返回套接字描述符，错误返回-1
int tcp_server_uici(int max_cli, int ser_port, char* ser_ip);

//注意：此函数只适用于Liunx系统下UDP协议的IPV4
//参数ser_port为服务器端口号
//参数ser_ip为服务器ip地址,如果为NULL，则由系统自动分配ip
//本函数成功返回套接字描述符，错误返回-1
int udp_server_uici(int ser_port, char* ser_ip);

//TCP：串行服务器
//该服务器只支持一个客户通讯,其他客户需要排队
//参数ser_sock是服务器套接字，需先创建套接字后才可使用该函数
//成功返回0，错误返回-1  
int tcp_server_serial(int* ser_sock);

//TCP:多进程服务器
//该服务器可支持多个客户进程通讯，但资源占用量大
//参数ser_sock是服务器套接字，需先创建套接字后才可使用该函数
//参数monitor是服务器的监听大小，客户超过该数值后需要等待客户下线才能连上
//该函数成功返回0，错误返回-1
int tcp_server_progress(int* ser_sock, int monitor); 

//TCP:多线程服务器
//该服务器可支持多个客户进程通讯，但资源占用量会比多进程的小
//参数ser_sock是服务器套接字，需先创建套接字后才可使用该函数
//参数monitor是服务器的监听大小，客户超过该数值后需要等待客户下线才能连上
//成功返回0，失败返回-1
int tcp_server_pthread(int* ser_sock, int monitor);

#endif

