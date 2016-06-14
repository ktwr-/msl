#include  <sys/types.h>
#include  <sys/socket.h>
#include  <stdio.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <unistd.h>
#include  <string.h>
#include  <stdlib.h>

#define PORT 6813

int main(int argc, char **argv)
{

  	int sockfd, len, result;
 	struct sockaddr_in address;
	char ch[1000];
	char rec[1000];
	int temp,size;
	
	bzero(rec,sizeof(rec));
	printf("scan for transport server\n");
	scanf("%s",ch);
	printf("char = %s\n",ch);
  	/*クライアント用ソケット作成*/
   	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
   	/*サーバ側と同じ名前でソケットの名前を指定*/
   	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = inet_addr("127.0.0.1");
    	address.sin_port = htons(PORT);
 	len = sizeof(address);

 	/*クライアントのソケットとサーバのソケットの接続*/
 	result = connect(sockfd, (struct sockaddr *)&address, len);
	if(result == -1) perror("connect");

    	/*sockfdを介して読み書きができるようにする*/
    	size = send(sockfd, ch, 1000,0);
	while(temp != size){
		temp += recv(sockfd, rec, 1000,0);	
		printf("%d %d\n",temp,size);
    	}
	printf("char from server = %s\n", rec);
	close(sockfd);
}
