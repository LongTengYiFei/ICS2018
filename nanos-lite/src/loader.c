#include "proc.h"

#define DEFAULT_ENTRY 0x4000000

static uintptr_t loader(PCB *pcb, const char *filename) {
  printf("come into loader!\n");
  printf("ramdisk size = %d\n",get_ramdisk_size());
  char *buf = NULL;
  ramdisk_read(buf, 0, get_ramdisk_size());
  printf("buf[0] = %d\n", buf[0]);
  printf("buf[1] = %d\n", buf[1]);
  printf("buf[2] = %d\n", buf[2]);
  printf("loader over!\n");
  printf("\n");
  TODO();
  return DEFAULT_ENTRY;
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
