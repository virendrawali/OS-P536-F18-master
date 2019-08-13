#include <xinu.h>
#include <future.h>

uint future_prod(future_t* fut,int n) {
  intmask mask;
  mask = disable();
  printf("Produced %d\n",n);
  future_set(fut, n);
  restore(mask);
  return OK;
}

uint future_cons(future_t* fut) {
  int i, status;
  intmask mask;
  mask = disable();
  status = (int)future_get(fut, &i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  //sleep(0.3);
  printf("Consumed %d\n", i);
  restore(mask);
  return OK;
}
