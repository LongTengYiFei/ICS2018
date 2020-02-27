#include "cpu/exec.h"

void difftest_skip_ref();
void difftest_skip_dut();

make_EHelper(lidt) {
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
  //printf("in begin!\n");
  //TODO();
  print_asm_template2(in);
  int read = pio_read_b(id_dest->val);
  printf("read is 0x%x\n",read);
  printf("dl is 0x%x\n",cpu.edx & 0x000000ff);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
  //printf("in over!\n");
}

make_EHelper(out) {
  //TODO();
  //printf("out begin!\n");
  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
  //printf("out over!\n");
}
