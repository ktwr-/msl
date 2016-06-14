#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#define STACK_NUM 5

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

static struct {
  int pos;
  struct {
    ucontext_t ctx;
    char stack[SIGSTKSZ];
  } proc[STACK_NUM];
} uctx;

static void next_uctx(int dummy)
{
  int old_uctx_pos = uctx.pos;

  uctx.pos = (uctx.pos + 1) % STACK_NUM;

  if (swapcontext(&uctx.proc[old_uctx_pos].ctx, &uctx.proc[uctx.pos].ctx) == -1)
    handle_error("swapcontext");
}

static void func(int id)
{
  int i, j;

  for(i = 0; i < 10; i++) {
    for(j = 0; j < 1000000000; j += (id + 1));
    printf("[id:%d]-[i:%d]-[j:%d]\n", id, i, j);
  }
}

int main(int argc, char *argv[])
{
  int i;
  ucontext_t uctx_main;
  struct sigaction sa, old_sa;
  struct itimerval itval;

  sa.sa_handler = next_uctx;
  sa.sa_flags = SA_RESTART;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, &old_sa) == -1)
      handle_error("sigaction");

  itval.it_interval.tv_sec = 0;
  itval.it_interval.tv_usec = 10000;
  itval.it_value = itval.it_interval;
  if (setitimer(ITIMER_REAL, &itval, 0) == -1)
      handle_error("setitimer");

  for(i = 0; i < STACK_NUM; i++) {
    ucontext_t *ucp = &uctx.proc[i].ctx;

    if (getcontext(ucp) == -1)
      handle_error("getcontext");

    ucp->uc_stack.ss_sp = uctx.proc[i].stack;
    ucp->uc_stack.ss_size = sizeof(uctx.proc[0].stack);
/* ucp->;uc_link = (i &gt;= STACK_NUM - 1) ? &uctx_main: &uctx.proc[i + 1].ctx; */
    ucp->uc_link = &uctx_main;
    makecontext(ucp, (void (*)(void))func, 1, i);
  }

  printf("----- start ----- \n");

  if (swapcontext(&uctx_main, &uctx.proc[0].ctx) == -1)
    handle_error("swapcontext");

  printf("----- end ----- \n");

  exit(EXIT_SUCCESS);
}
