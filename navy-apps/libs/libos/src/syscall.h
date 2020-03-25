#ifndef __SYSCALL_H__
#define __SYSCALL_H__

enum {
  SYS_exit,//0
  SYS_yield,//1
  SYS_open,
  SYS_read,
  SYS_write,//4
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,//9
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

#endif
