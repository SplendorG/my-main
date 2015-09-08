
#include "server.h"

//信号检测变量
static int sig = 0;
//跟踪TCP服务器客户连接量
static int ser_listen = 1;
//该结构体为TCP多线程客户的地址信息
static struct sockaddr_in pth_cli_addr = {0};

//该信号函数用于监测客户机断开信号，改变信号变量
static void test_sig()
{
	sig = 1;
	return;
}

//该信号函数用于处理子进程回收
static void cli_progress()
{
	ser_listen--;
	waitpid(-1, NULL, WNOHANG);
	return;
}

//创建服务器的通用函数,此函数只适用于Liunx系统中TCP协议的IPV4模式
//参数max_cli表示服务器所能接受的最大客户量
//参数ser_port为服务器端口号
//参数ser_ip为服务器ip地址,如果为NULL，则由系统自动分配ip
//本函数成功返回套接字描述符，错误返回-1
int tcp_server_uici(int max_cli, int ser_port, char* ser_ip)
{
	int ser_sock = 0;
	int val = 1;
	struct sockaddr_in ser_addr = {0};
	socklen_t ser_len = sizeof(struct sockaddr_in);

	//监测客户端断开信号
	signal(SIGPIPE, test_sig);

	//创建套接字
	ser_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(-1 == ser_sock)
	{
		perror("TCP:server socket failed");
		return -1;
	}

	//设置服务器端口可重用
	if(-1 == setsockopt(ser_sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
	{
		perror("setsockopt server failed");
		if(ser_sock != -1)
		{
			close(ser_sock);
		}
		return -1;
	}

	//把端口号和IP地址转换成网络字节序
	ser_addr.sin_family = PF_INET;
	//判断端口号类型，转换网络字节序
	if(ser_port < 65536)
	{
		ser_addr.sin_port = htons(ser_port);
	}
	else
	{
		ser_addr.sin_port =htonl(ser_port);	
	}

	//对参数来决定是否自动获取IP
	if(NULL == ser_ip)
	{
		ser_addr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		ser_addr.sin_addr.s_addr = inet_addr(ser_ip);
	}

	//绑定套接字到端口
	if(-1 == bind(ser_sock, (struct sockaddr*)&ser_addr, ser_len))
	{
		perror("TCP:server bind failed");
		if(ser_sock != -1)
		{
			close(ser_sock);
		}
		return -1;
	}

	//服务器监听上限设置
	if(-1 == listen(ser_sock, max_cli))
	{
		perror("TCP:server listen failed");
		if(ser_sock != -1)
		{
			close(ser_sock);
		}
		return -1;
	}	

	return ser_sock;
}

//创建服务器的通用函数，此函数只适用于Liunx系统下UDP协议的IPV4
//参数ser_port为服务器端口号
//参数ser_ip为服务器ip地址,如果为NULL，则由系统自动分配ip
//本函数成功返回套接字描述符，错误返回-1
int udp_server_uici(int ser_port, char* ser_ip)
{
	int val = 1;
	int ser_sock = 0;
	struct sockaddr_in ser_addr;
	socklen_t ser_len = sizeof(struct sockaddr_in);	

	//创建套接字
	ser_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(-1 == ser_sock)
	{
		perror("UDP:server socket failed");
		return -1;
	}	

	//设置服务器端口可重用
	if(-1 == setsockopt(ser_sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
	{
		perror("setsockopt server failed");
		if(ser_sock != -1)
		{
			close(ser_sock);
		}
		return -1;
	}

	//把端口号和IP地址转换成网络字节序
	ser_addr.sin_family = PF_INET;
	//判断端口号类型，转换网络字节序
	if(ser_port < 65536)
	{
		ser_addr.sin_port = htons(ser_port);
	}
	else
	{
		ser_addr.sin_port =htonl(ser_port);	
	}

	//对参数来决定是否自动获取IP
	if(NULL == ser_ip)
	{
		ser_addr.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		ser_addr.sin_addr.s_addr = inet_addr(ser_ip);
	}

	//绑定套接字到端口
	if(-1 == bind(ser_sock, (struct sockaddr*)&ser_addr, ser_len))
	{
		perror("UDP:server bind failed");
		if(ser_sock != -1)
		{
			close(ser_sock);
		}
	}

	return ser_sock;
}

//TCP：串行服务器
//该服务器只支持一个客户通讯
//参数ser_sock是服务器套接字，需先创建套接字后才可使用该函数
//成功返回0，错误返回-1
int tcp_server_serial(int* ser_sock)
{
	while(1)
	{
		int cli_sock = 0;
		char buf_r[128] = "\0";
		char buf_w[128] = "Welcome to my server.Thanks!!";
		struct sockaddr_in cli_addr = {0};
		socklen_t cli_len = sizeof(struct sockaddr_in);

		//等待跟客户机进行连接
		cli_sock = accept(*ser_sock, (struct sockaddr*)&cli_addr, &cli_len);
		if(-1 == cli_sock)
		{
			perror("client addept failed");
			if(*ser_sock != -1)
			{
				close(*ser_sock);
			}
			return -1;
		}
		printf("with for client link OK!!\n");

		//跟客户机进行一对一连接，此时不能有其他客户机连接
		while(1)
		{
			//根据触发客户机关闭信号改变变量值，跳出循环
			if(sig == 1)
			{
				printf("The client is close!!\n");
				sig = 0;
				close(cli_sock);
				break;
			}

			//读取客户发来的信息
			if(-1 == read(cli_sock, buf_r, sizeof(buf_r)))
			{
				perror("from client failed");
				close(*ser_sock);
				close(cli_sock);
				return -1;
			}

			//判断客户机端口号长度，打印客户机的IP和端口
			if(ntohs(cli_addr.sin_port) < 65536)
			{
				printf("from client news,the IP = %s\tthe PORT = %d\n",
						inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
			}
			else
			{
				printf("from client news,the IP = %s\tthe PORT = %d\n",
						inet_ntoa(cli_addr.sin_addr), ntohl(cli_addr.sin_port));
			}

			//打印客户机回应的信息，并清空缓存
			printf("%s\n", buf_r);
			memset(buf_r, 0, sizeof(buf_r));

			//向客户机回应
			write(cli_sock, buf_w, strlen(buf_w));
		}
	}
	return 0;
}

//TCP:多进程服务器
//该服务器可支持多个客户进程通讯，但资源占用量大
//参数ser_sock是服务器套接字，需先创建套接字后才可使用该函数
//参数monitor是服务器的监听大小，客户超过该数值后需要等待客户下线才能连上
//该函数成功返回0，错误返回-1
int tcp_server_progress(int* ser_sock, int monitor) 
{
	int cli_sock = 0;
	pid_t progress = -1;
	char buf_r[128] = "\0";
	char server_max[128] = "Sorry!!Server is full!!Please wait other client inserting!!";
	char buf_w[128] = "Welcome to my server.Thanks!!";
	struct sockaddr_in cli_addr = {0};
	socklen_t cli_len = sizeof(struct sockaddr_in);

	signal(SIGCHLD, cli_progress);

	while(1)
	{
		//等待跟客户机进行连接
		cli_sock = accept(*ser_sock, (struct sockaddr*)&cli_addr, &cli_len);
		if(-1 == cli_sock)
		{
			perror("client addept failed");
			if(*ser_sock != -1)
			{
				close(*ser_sock);
			}
			return -1;
		}

		//判断服务器是否已满
		if(monitor < ser_listen)
		{
			//向客户机回应服务器已满
			write(cli_sock, server_max, strlen(server_max));
			close(cli_sock);	
			continue;
		}

		ser_listen++;
		printf("with for client link OK!!\n");

		//连接成功后，创建进程，子进程与客户机进行通讯
		progress = fork();
		if(-1 == progress)
		{
			perror("fock failed");
			return -1;
		}
		//转到子进程通讯
		else if(0 == progress)
		{
			if(*ser_sock != -1)
			{
				close(*ser_sock);
			}
			//跟客户机进行一对一连接，此时不能有其他客户机连接
			while(1)
			{
				//根据触发客户机关闭信号改变变量值，跳出循环
				if(sig == 1)
				{
					printf("The client is close!!\n");
					sig = 0;
					close(cli_sock);
					break;
				}

				//读取客户发来的信息
				if(-1 == read(cli_sock, buf_r, sizeof(buf_r)))
				{
					perror("from client failed");
					close(cli_sock);
					return -1;
				}

				//判断客户机端口号长度，打印客户机的IP和端口
				if(ntohs(cli_addr.sin_port) < 65536)
				{
					printf("from client news,the IP = %s\tthe PORT = %d\n",
							inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
				}
				else
				{
					printf("from client news,the IP = %s\tthe PORT = %d\n",
							inet_ntoa(cli_addr.sin_addr), ntohl(cli_addr.sin_port));
				}

				//打印客户机回应的信息，并清空缓存
				printf("%s\n", buf_r);
				memset(buf_r, 0, sizeof(buf_r));

				//向客户机回应
				write(cli_sock, buf_w, strlen(buf_w));
			}
		}
		//主进程关闭客户机接口，继续下一个客户的连接
		else if(progress > 0)
		{
			close(cli_sock);
		}
	}

	return 0;
}

//此函数是线程调用，用于跟客户之间进行数据交换
//参数pth_cli_sock是客户机的接口
static int pthread(int pth_cli_sock)
{
	int cli_sock = (int)pth_cli_sock;
	char buf_r[128] = "\0";
	char buf_w[128] = "Welcome to my server.Thanks!!";

	//跟客户机进行一对一连接，此时不能有其他客户机连接
	while(1)
	{
		//根据触发客户机关闭信号改变变量值，跳出循环
		if(sig == 1)
		{
			printf("The client is close!!\n");
			sig = 0;
			close(cli_sock);
			break;
		}

		//读取客户发来的信息
		if(-1 == read(cli_sock, buf_r, sizeof(buf_r)))
		{
			perror("from client failed");
			close(cli_sock);
			return -1;
		}

		//判断客户机端口号长度，打印客户机的IP和端口
		if(ntohs(pth_cli_addr.sin_port) < 65536)
		{
			printf("from client news,the IP = %s\tthe PORT = %d\n",
					inet_ntoa(pth_cli_addr.sin_addr), ntohs(pth_cli_addr.sin_port));
		}
		else
		{
			printf("from client news,the IP = %s\tthe PORT = %d\n",
					inet_ntoa(pth_cli_addr.sin_addr), ntohl(pth_cli_addr.sin_port));
		}

		//打印客户机回应的信息，并清空缓存
		printf("%s\n", buf_r);
		memset(buf_r, 0, sizeof(buf_r));

		//向客户机回应
		write(cli_sock, buf_w, strlen(buf_w));
	}
}

//TCP:多线程服务器
//该服务器可支持多个客户进程通讯，但资源占用量会比多进程的小
//参数ser_sock是服务器套接字，需先创建套接字后才可使用该函数
//参数monitor是服务器的监听大小，客户超过该数值后需要等待客户下线才能连上
//成功返回0，失败返回-1
int tcp_server_pthread(int* ser_sock, int monitor)
{
	int cli_sock = 0;
	pthread_t pthread = 0;
	socklen_t cli_len = sizeof(struct sockaddr_in);
	char server_max[128] = "Sorry!!Server is full!!Please wait other client inserting!!";

	while(1)
	{
		//等待跟客户机进行连接
		cli_sock = accept(*ser_sock, (struct sockaddr*)&pth_cli_addr, &cli_len);

		if(-1 == cli_sock)
		{
			perror("client addept failed");
			if(*ser_sock != -1)
			{
				close(*ser_sock);
			}
			return -1;
		}

		//判断服务器是否已满
		if(monitor < ser_listen)
		{
			//向客户机回应服务器已满
			write(cli_sock, server_max, strlen(server_max));
			close(cli_sock);	
			continue;
		}

		ser_listen++;
		printf("with for client link OK!!\n");

		//与客户机连接成功后，交由子线程处理
		//创建子线程
		if(0 != pthread_create(&pthread, NULL, (void*)pthread, (void*)&cli_sock))
		{
			perror("server pthread_create failed");
			if(*ser_sock != -1)
			{
				close(*ser_sock);
			}
			return -1;
		}
	}

	return 0;
}
