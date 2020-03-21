#include "common.h"
#include "syscall.h"
#include "proc.h"
uintptr_t sys_yield();
void sys_exit(int exit_code);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  // i add this
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
 /* 
  printf("come into do syscall!\n");
  printf("a[0] = 0x%x\n",a[0]);
  printf("a[1] = 0x%x\n",a[1]);
  printf("a[2] = 0x%x\n",a[2]);
  printf("a[3] = 0x%x\n",a[3]);
  printf("\n");
  */
  switch (a[0]) {
    case SYS_exit: sys_exit(a[0]); break;
    case SYS_yield: c->GPR1 = sys_yield();break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}

uintptr_t sys_yield(){
  _yield();
  return 0;
}
void sys_exit(int exit_code){
  _halt(exit_code);
}
uintptr_t sys_write(){

   return 0;
}
