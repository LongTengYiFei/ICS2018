#include "cpu/exec.h"

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
/*
  printf("come into lidt\n");
  printf("id_dest->addr = 0x%x\n",id_dest->addr);
  printf("id_dest->width = 0x%x\n",id_dest->width);
*/

 /**
   IF OperandSize = 16
   THEN IDTR.Limit:Base <- m16:24 (* 24 bits of base loaded *)
   ELSE IDTR.Limit:Base <- m16:32
   FI;
  */ 
  cpu.idtr.base = vaddr_read(id_dest->addr + 2, 4);
  cpu.idtr.len = vaddr_read(id_dest->addr, 2); 
  printf("cpu.idtr.base = 0x%x\n",cpu.idtr.base);
  printf("cpu.idtr.len = 0x%x\n",cpu.idtr.len);
  //printf("over lidt\n");
  TODO();
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  printf("come into int\n");
  printf("id_dest->type = 0x%x\n",id_dest->type);
  printf("id_dest->val = 0x%x\n",id_dest->val);
  printf("decoding.seq_eip = 0x%x\n",decoding.seq_eip);
  uint8_t NO = id_dest->val;
  vaddr_t ret_addr = decoding.seq_eip;
  raise_intr(NO, ret_addr);
  printf("int over\n");
  TODO();
  print_asm("int %s", id_dest->str);
#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
  TODO();

  print_asm("iret");
}

make_EHelper(in) {
  //TODO();
  print_asm_template2(in);
/*
  printf("id_src->val is 0x%x\n",id_src->val);
  printf("id_dest->val is 0x%x\n",id_dest->val);
  printf("dx is 0x%x\n",cpu.edx & 0x0000ffff);
  printf("eax is 0x%x\n",cpu.eax);
*/
  
 // printf("in id_dest->width is 0x%x\n",id_dest->width);
 // printf("in id_src->width is 0x%x\n",id_src->width);
  if(id_src->width == 1)
    reg_b(R_AL) = pio_read_b(id_src->val);
  else if(id_src->width == 2)
    reg_w(R_AX) = pio_read_w(id_src->val);
  else if(id_src->width == 4)
    reg_l(R_EAX) = pio_read_l(id_src->val);
  //printf("port dx is 0x%x\n",cpu.edx & 0x0000ffff);
  //printf("id_src->val is 0x%x\n",id_src->val);
#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  //TODO();
  print_asm_template2(out);
  /*
  printf("out src width is 0x%x\n",id_src->width);
  printf("out src value is 0x%x\n",id_src->val);
  printf("out dest value is 0x%x\n",id_dest->val);
  printf("port dx is 0x%x\n",cpu.edx & 0x0000ffff);
  printf("eax is 0x%x\n",cpu.eax);*/

  if(id_src->width == 1)
    pio_write_b(id_dest->val,id_src->val);
  else if(id_src->width == 2)
    pio_write_w(id_dest->val,id_src->val);
  else if(id_src->width == 4)
    pio_write_l(id_dest->val,id_src->val);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
