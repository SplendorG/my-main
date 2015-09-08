
#include "server.h"

int main(void)
{
	volatile int ser_sock = 0;
	volatile struct sockaddr cli_addr = {0};
	volatile char r_buff[] = "\0";
	const char w_buff[] = "hello client";
	const socklen_t cli_len = sizeof(struct sockaddr);	
	
	//创建套接字
	ser_sock = 

	while(1)
	{
		recvfrom(ser_sock, r_buff, sizeof(r_buff), 0,
				&cli_addr, &cli_len);

		sendto(ser_sock, w_buff, strlen(w_buff), 0,
				&cli_addr, cli_len);

		printf("from client Msg:%s\n", r_buff);
	}

	return 0;
}
