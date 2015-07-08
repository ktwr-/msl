#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "mythread.h"

#define SIZE 8 

struct _quick{
	int left;
	int right;
};

int a[SIZE];

int med3(int x,int y,int z){
	if(x<y){
		if (y < z){
			 return y;
		} else if (z < x){
			 return x;
		}else{
			 return z;
		}
	}else{
		if (z < y){
			return y; 
		}else if (x < z){
			return x;
		}else {
			return z;
		}
	}
}

void quicksort(void *data){
	struct _quick *sort = (struct _quick *)data;
	int i,j;
	int tmp,pivot;
	struct _quick t1,t2;
	int id1,id2;
	//printf("start quick\n");
	if(sort[0].left < sort[0].right){
		i = sort[0].left;
		j = sort[0].right;	
		pivot = med3(a[i],a[i + (j-i)/2],a[j]);
		while(1){
			while (a[i] < pivot) i++; /* a[i] >= pivot となる位置を検索 */
            		while (pivot < a[j]) j--; /* a[j] <= pivot となる位置を検索 */
            		if (i >= j) break;
			mutex();
            		tmp = a[i];
			a[i] = a[j];
			a[j] = tmp;
			mutex();
            		i++; j--;
        	}
		t1.left = sort[0].left;
		t1.right = i-1;
		t2.left = j+1;
		t2.right = sort[0].right;
		id1=mythread_create(quicksort,&t1);
		id2=mythread_create(quicksort,&t2); 
		mythread_join(id1);
		mythread_join(id2);
	}	
	
}

int main(void){
	int id;
	int i;
	struct _quick first;
	printf("type %d data\n",SIZE);
	for(i=0;i<SIZE;i++){
		scanf("%d",&a[i]);
	}
	first.left=0;
	first.right = SIZE-1;
	mythread_init();
	id = mythread_create(quicksort,&first);
	mythread_join(id);
	printf("sort data\n");
	for(i=0;i<SIZE;i++){
		printf("%d\n",a[i]);
	}
	return 0;
}
