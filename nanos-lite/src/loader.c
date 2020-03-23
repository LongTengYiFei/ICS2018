#include "proc.h"
#include "fs.h"
#define DEFAULT_ENTRY 0x4000000

static uintptr_t loader(PCB *pcb, const char *filename) {
  uint32_t entry = 0;
  //we can ignore the flags and mode
  int fd = fs_open(filename, 0, 0);
  printf("fd = %d\n",fd);
  size_t len = fs_read(fd, (void*)DEFAULT_ENTRY, fs_filesz(fd));//all of this file
  printf("len = %d\n",len);
  fs_close(fd);
  return entry;
}

void naive_uload(PCB *pcb, const char *filename) { 
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
