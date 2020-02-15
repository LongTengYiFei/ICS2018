#include "cpu/exec.h"

make_EHelper(mov) {

  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
 //printf("Come into EHelper(push)!\n");
 //printf("id_dest->width=%d\n",id_dest->width);
  if(id_dest->width == 4)
    rtl_push(&id_dest->val);
  else assert(0);

  print_asm_template1(push);
  //printf("EHelper(push) over\n");
}

make_EHelper(pop) {
  //printf("Come into EHelper(pop)!\n");
  rtl_pop(&t0);
  if(id_dest->width == 4)
    id_dest->val = t0;
  else assert(0);
  //printf("id_dest->reg = %d\n",id_dest->reg);//id_dest->reg is which reg
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
  //printf("EHelper(pop) over!\n");
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  printf("leave begin!\n");
  printf("EBP = 0x%x ",cpu.ebp);
  //printf("BP = 0x%x\n",cpu.bp);
  printf("ESP = 0x%x ",cpu.esp);
  //printf("SP = 0x%x\n",cpu.sp);


  printf("leave over!\n");
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {//load effective address
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
