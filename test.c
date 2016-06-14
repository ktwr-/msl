#include<stdio.h>
#include<stdlib.h>
#include "mythread.h"

void f1(void *args){
	int i,j;
	char *str = (char*)args;
	for(i=0;i<10;i++){
		printf("%s\n",str);
		for(j=0; j<10000000; j+= 1 + rand()%2);
	}
}


int main(int argc,char **argv){
	int t1,t2;
	mythread_init();
	t1 = mythread_create(f1,"kin");
	t2 = mythread_create(f1,"patsu");
	mythread_join(t1);
	printf("test\n");
	mythread_join(t2);
	
	return 0;
}
