#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  	
  //printf("come into raise_intr\n");
  //printf("NO is 0x%x\n",NO);
  rtl_push(&cpu.eflags.value);
  cpu.eflags.IF = 0;
  rtl_push(&cpu.cs);
  //printf("ret_addr is 0x%x\n",ret_addr);
  rtl_push(&ret_addr);
  
  t0 = cpu.idtr.base;
  //one GD is 8 bytes
  //zhu yi!!
  uint32_t offset_low = vaddr_read(t0 + NO*8, 2);
  uint32_t offset_high = vaddr_read(t0 + NO*8 + 4, 4);
  
  offset_high = offset_high & 0xffff0000;
  uint32_t dest = offset_high + offset_low;
  printf("dest = 0x%x\n",dest);
  rtl_jr(&dest);
  //TODO();
  //printf("raise_intr over\n\n");
}

void dev_raise_intr() {
}
