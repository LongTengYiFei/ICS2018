#include "proc.h"

#define DEFAULT_ENTRY 0x4000000

static uintptr_t loader(PCB *pcb, const char *filename) {
  printf("come into loader!\n");
  printf("ramdisk size = %d\n",get_ramdisk_size());
  char *buf = NULL;
  int ramdisk_size = get_ramdisk_size();
  ramdisk_read(buf, 0, ramdisk_size);
 /* 
  for(int i=0;i<=ramdisk_size-1;i++)
	  vaddr_write(0x4000000 + i, *(buf+i), 1);*/
  printf("loader over!\n");
  printf("\n");
  //TODO();
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
