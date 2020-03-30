#include "common.h"
#include "syscall.h"
#include "proc.h"
#include "fs.h"
uintptr_t sys_yield();
void sys_exit(int exit_code);
uintptr_t sys_write(int fd,  void* buf, size_t len);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  // i add this
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  
  switch (a[0]) {
    case SYS_exit: naive_uload(NULL, "/bin/init"); break;
    case SYS_yield: c->GPR1 = sys_yield(); break;
    case SYS_write: c->GPR1 = sys_write(a[1], (void*)a[2], a[3]); break;
    case SYS_brk: c->GPR1 = 0; break; 
    case SYS_open: c->GPR1 = fs_open((void*)a[1], a[2], a[3]); break;
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
uintptr_t sys_write(int fd,  void* buf, size_t len){
   //printf("come into sys write\n");
   return fs_write(fd, buf, len);
}
