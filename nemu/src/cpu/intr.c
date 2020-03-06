#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
	/*
  printf("come into raise_intr\n");
  rtl_push(&cpu.eflags);
  rtl_push(&cpu.cs);
  rtl_push(&cpu.eip);
  
  t0 = cpu.idtr.base;
  //one GD is 8 bytes
  GateDesc g0 = vaddr_read(t0 + NO*8, 8);
  uint32_t offset_low = g0.off_15_0;
  uint32_t offset_high = g0.off_31_16;
  
  offset_high = offset_high << 16;
  uint32_t dest = offset_high + offset_low;
  //TODO();
  printf("raise_intr over\n");*/
}

void dev_raise_intr() {
}
