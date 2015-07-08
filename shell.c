#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "mythread.h"

#define SIZE 8

int a[SIZE];
int tsize=SIZE;

struct _data{
	int start;
	int h;
};

void insertsort(void *data){
	struct _data *no = (struct _data *)data;
	int h= no[0].h;
	int start=no[0].start;
	int i,j;
	int tmp;
	for (i = start+h; i < SIZE; i+=h) {
    		tmp = a[i];
    		if (a[i - h] > tmp) {
        		j = i;
        		do {
            			a[j] = a[j - h];
            			j-=h;
        		} while (j > 0 && a[j - h] > tmp);
        		a[j] = tmp;
    		}
	}

}
void sort(int h){
	int i;
	int id[10];
	for(i=0;i<h;i++){
		struct _data no[100];
		no[i].start=i;
		no[i].h=h;
		id[i] = mythread_create(insertsort,&no[i]);
	}
	
	for(i=0;i<h;i++){
		mythread_join(id[i]);
	}
	for(i=0;i<SIZE;i++){
		printf("%d ",a[i]);
	}
	printf("\n");
}



int main(void){
	int i;
	int id;
	int h = SIZE;
	printf("type %d data\n",SIZE);
	for(i=0;i<SIZE;i++){
		scanf("%d",&a[i]);
	}
	while(h != 1){
		h = tsize / 2;
		sort(h);
		tsize = h;
	}
	printf("sort data\n");
	for(i=0;i<SIZE;i++){
		printf("%d ",a[i]);
	}
	printf("\n");
	return 0;
}
