#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <assert.h>
#include<malloc.h>

#define MAX_THREAD 32
#define FRAME_SIZE 10240

#define DEAD 0
#define ALIVE 1
#define JOIN 2

struct thread_data {
	ucontext_t ucp;
	int state;
	int wait;
	char stack[16384];
	};

struct thread_data thread_table[MAX_THREAD];
static int current_thread = 0;
static int num_thread =1;
static const unsigned int STACK_SIZE = 16*1024;
static int flag =0;

static int search_alive_thread()
{
	int i;
	for(i=current_thread+1; i<num_thread;i++){
		if(thread_table[i].state == ALIVE) {
			return i;
		}
	}
	for(i=0;i<=current_thread;i++){
		if(thread_table[i].state == ALIVE ){
			return i;
		}
	}
	return -1;
}

static int search_free_thread(){
	int i = num_thread++;
	assert(i < MAX_THREAD);

	return i;
}

static int search_join_thread(int id){
	int i;
	for(i=0;i<num_thread;i++){
		if(thread_table[i].state == JOIN && thread_table[i].wait == id){
			return i;
		}
	}	
	return -1;
}
void mythread_yield(){
	int next = search_alive_thread();
	int temp = current_thread;
	current_thread = next;
	if(next != -1 && temp  != next){
		swapcontext(&thread_table[temp].ucp,&thread_table[next].ucp);
	}
}

/*排他制御mymutex*/

void mutex(){
	if(flag == 0){
		flag =1;
	}else{
		flag =0;
	}

}
void mythread_join(int id){
	if(thread_table[id].state != DEAD){
		thread_table[current_thread].state = JOIN;
		thread_table[current_thread].wait = id;
		mythread_yield();
	}

}

void mythread_exit(int id){
	int join;
	if(current_thread == 0){
		exit(0);
	}
	join = search_join_thread(current_thread);
	if( join != -1){
		thread_table[join].state = ALIVE;
	}
	thread_table[current_thread].state = DEAD;
	mythread_yield();
}
void do_func(void (*func)(void *),void *args){
	func(args);
	/*
	thread_table[current_thread].state = DEAD;
	if(thread_table[0].wait == current_thread){
		thread_table[0].state = ALIVE;
		thread_table[0].wait = 0;
	}
	mythread_yield();
	*/
	mythread_exit(current_thread);
}

/*スレッドを作る*/
int mythread_create(void (*func)(void *),void *args){
	mutex();
	int id = search_free_thread();
	thread_table[id].state = ALIVE;
	if(getcontext(&thread_table[id].ucp) == 0){
		thread_table[id].ucp.uc_stack.ss_sp=thread_table[id].stack;
		thread_table[id].ucp.uc_stack.ss_size=sizeof(thread_table[id].stack);
		thread_table[id].ucp.uc_link == &thread_table[0].ucp;
		makecontext(&thread_table[id].ucp,(void (*)(void))do_func,2,(void *)func,args);
	}else{
		assert(0);
	}
	mutex();
	return id;
}

static void mythread_handler(int sig){
	if(num_thread > 1 && flag ==0){
		mythread_yield();
	}
}

static void mythread_init(){	
	struct sigaction sa = {
		.sa_handler = mythread_handler,
		.sa_flags = SA_RESTART
	};
	struct itimerval it={};
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 100000;
	it.it_value = it.it_interval;
	sigemptyset(&sa.sa_mask);
	if(sigaction(SIGALRM,&sa,NULL) < 0){
		perror("sigaction error");
		exit(1);
	}
	if(setitimer(ITIMER_REAL,&it,0) < 0){
		perror("setitimer error");
		exit(1);
	}
	thread_table[0].state = ALIVE;
        if(getcontext(&thread_table[0].ucp) == 0){
                thread_table[0].ucp.uc_stack.ss_sp=thread_table[0].stack;
                thread_table[0].ucp.uc_stack.ss_size=sizeof(thread_table[0].stack);
                thread_table[0].ucp.uc_link == NULL;
	}

}
