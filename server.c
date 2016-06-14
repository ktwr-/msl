#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include "mythread.h"

#define PORT 6813
void myrecv(void *sockfd);

int main(void){
	int server_sockfd, client_sockfd;
    	int server_len, client_len;
    	struct sockaddr_in server_address;
    	struct sockaddr_in client_address;
	int size;
	int t[32];
	int i,j;	
    	/*サーバ用ソケット作成*/
    	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    	/*ソケットに名前をつける(bind)*/
    	server_address.sin_family = AF_INET;
    	server_address.sin_addr.s_addr = INADDR_ANY;
    	server_address.sin_port = htons(PORT);
    	server_len = sizeof(server_address);
    	bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	
    	/*接続キューを作成しクライアントからの接続を待つ*/
    	listen(server_sockfd, 5);
	mythread_init();
    	while(1)
    	{
        	char ch[1000];
        	printf("server waiting\n");

        /*接続を受け入れる*/
        client_sockfd = accept(server_sockfd, 
		(struct sockaddr *)&client_address, &client_len);

        /*client_sockfdを介してクライアントに対する読み書きができるようになる*/
        //recv(client_sockfd, ch, 1000,0);
        //send(client_sockfd, ch, 1000,0);
        //close(client_sockfd);
        t[i]=mythread_create(myrecv,&client_sockfd);
	i++;
    }
	for(j=0;j<32;j++){
		mythread_exit(t[j]);
	}
}
void myrecv(void *fd){
	int *sockfd = (int*)fd;
	char buf[1000];
	int size;
	printf("sockfd = %d\n",sockfd[0]);
	size=recv(sockfd[0],buf,1000,0);
	sleep(1);
	send(sockfd[0],buf,1000,0);
}
