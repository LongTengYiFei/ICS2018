#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  TODO();

  print_asm_template2(test);
}

make_EHelper(and) {
  printf("begin and!\n");
  //cf =0,sf =0
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  printf("id_dest->val = 0x%x ",id_dest->val);
  printf("id_src->val = 0x%x\n",id_src->val);
  printf("cpu.esp = 0x%x\n",cpu.esp);
  //opration and
  rtl_and(&t1, &id_dest->val, &id_src->val);
  printf("t1 = 0x%x\n",t1);
  //zf sf  
  rtl_update_ZFSF(&t1, id_dest->width);
  //write back
  operand_write(id_dest, &t1);
  printf("write back ,cpu.esp  = 0x%x\n",cpu.esp);
  printf("and over!\n");

  print_asm_template2(and);
}

make_EHelper(xor) {
  TODO();

  print_asm_template2(xor);
}

make_EHelper(or) {
  TODO();

  print_asm_template2(or);
}

make_EHelper(sar) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
