
#include "client.h"

int main(int argv, char** argc)
{
	volatile int cli_sock = 0;
	struct sockaddr_in ser_addr = {0};
	char r_buff[] = "\0";
	char w_buff[] = "\0";   	
	socklen_t ser_len = sizeof(struct sockaddr_in);

	assert(2 == argv);
	
	cli_sock = udp_client_uici();
	
	ser_addr.sin_family = PF_INET;
	ser_addr.sin_port = htons(PORT);
	ser_addr.sin_addr.s_addr = inet_addr(argc[1]);

	while(1)
	{
		printf("请输入信息：");
		scanf("%s", w_buff);

		sendto(cli_sock, w_buff, strlen(w_buff), 0,
				(struct sockaddr*)&ser_addr, ser_len);
		recvfrom(cli_sock, r_buff, sizeof(r_buff), 0,
				(struct sockaddr*)&ser_addr, &ser_len);
		printf("%s\n", r_buff);
	}

	return 0;
}
