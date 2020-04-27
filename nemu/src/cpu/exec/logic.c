#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  //printf("test come in!\n");
  rtl_and(&t1, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t1, id_dest->width);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  
  rtl_update_PF(&t1);
  cpu.eflags.AF = 0;
  //printf("test over!\n");

  print_asm_template2(test);
}

make_EHelper(and) {
  //printf("begin and!\n");
  //cf =0,sf =0
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  /*
  printf("id_dest->val = 0x%x ",id_dest->val);
  printf("id_src->val = 0x%x\n",id_src->val);
  printf("id_dest->width = %d ",id_dest->width);
  printf("id_src->width = %d\n",id_src->width);*/
  //opration and
  rtl_and(&t1, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t1, id_dest->width);
  rtl_update_PF(&t1);
  cpu.eflags.AF = 0;
  operand_write(id_dest, &t1);

  print_asm_template2(and);
}

make_EHelper(xor) {
	/*
  printf("xor come in!\n");
  printf("id_dest->width = 0x%x ",id_dest->width);
  printf("id_src->width = 0x%x\n",id_dest->width);
  printf("id_dest->val = 0x%x ",id_dest->val);
  printf("id_src->val = 0x%x\n",id_src->val);
  printf("id_dest->reg = %d\n",id_dest->reg);
  printf("id_src->reg = %d\n",id_src->reg);
*/
  rtl_xor(&t1, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t1, id_dest->width);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  operand_write(id_dest, &t1);
  //printf("t1 = result = 0x%x\n",t1);
  //printf("xor over!\n");

  print_asm_template2(xor);
}

make_EHelper(or) {
	/*
  printf("or come in!\n");
  printf("id_dest->width = 0x%x ",id_dest->width);
  printf("id_src->width = 0x%x\n",id_dest->width);
  printf("id_dest->val = 0x%x ",id_dest->val);
  printf("id_src->val = 0x%x\n",id_src->val);
  printf("id_dest->reg = %d\n",id_dest->reg);
  printf("id_src->reg = %d\n",id_src->reg);
*/
  rtl_or(&t1, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t1, id_dest->width);

  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  rtl_update_PF(&t2);
  operand_write(id_dest, &t1);
  //printf("t1 = result = 0x%x\n",t1);
  //printf("or over!\n");
  print_asm_template2(or);
}

make_EHelper(sar) {
  //shift arith right
  /*
  printf("sar come in!\n");
  printf("id_dest->val = 0x%x ",id_dest->val);
  printf("id_src->val = 0x%x\n",id_src->val);
  printf("id_dest->width = %d ",id_dest->width);
  printf("id_src->width = %d\n",id_src->width);
  */
  if(id_dest->width == 1)
	id_dest->val = (int8_t)id_dest->val;
  if(id_dest->width == 2)
	id_dest->val = (int16_t)id_dest->val;

  rtl_sar(&t2, &id_dest->val, &id_src->val);
  //printf("t2 = result = 0x%x\n",t2);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  
  //CF OF PF  20200427
  rtl_update_PF(&t2);
  
  t3 = id_dest->val;
  for(int i=1;i<=id_src->val;i++){
	  cpu.eflags.CF = t3 & 0x1;
          t3 = t3 >> 1; 
  }

  print_asm_template2(sar);
}

make_EHelper(shl) {
  /*
  printf("shl come in!\n");
  printf("id_dest->val = 0x%x ",id_dest->val);
  printf("id_src->val = 0x%x\n",id_src->val);
  printf("id_dest->width = %d\n",id_dest->width);
  */
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  //printf("t2 = result = 0x%x\n",t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  //printf("shl over\n");
  // unnecessary to update CF and OF in NEMU

  rtl_update_PF(&t2);
  cpu.eflags.AF = 0;
  //CF
  t3 = id_dest->val;
  for(int i=1;i<=id_src->val;i++){
	  cpu.eflags.CF = t3 & 0x80000000;
          t3 = t3 << 1; 
  }

  print_asm_template2(shl);
}

make_EHelper(shr) {/*
  printf("shr come in!\n");
  printf("id_dest->val = 0x%x ",id_dest->val);
  printf("id_src->val = 0x%x\n",id_src->val);
  printf("id_dest->width = %d\n",id_dest->width);*/
  rtl_shr(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  //printf("t2 = result = 0x%x\n",t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  //printf("shr over\n");
  //TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  //ben lai jiu xie hao de  dai ma
  //printf("setcc begin!\n");
  uint32_t cc = decoding.opcode & 0xf;
  //printf("id_dest->type = %d\n",id_dest->type);
  //printf("id_dest->reg = %d\n",id_dest->reg);
  rtl_setcc(&t2, cc);
  //printf("t2 = 0x%x\n",t2);
  operand_write(id_dest, &t2);
  //printf("cpu.al= 0x%x\n",cpu.gpr[0]._8[0]);
  //printf("setcc over!\n");
  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {/*
  printf("not come in!\n");
  printf("id_dest->width = %d\n",id_dest->width);
  printf("id_dest->val =0x%x\n",id_dest->val);*/
  rtl_not(&t2, &id_dest->val);
  operand_write(id_dest, &t2);
  //printf("t2 = result = 0x%x\n",t2);  
  //printf("not over!\n");
  //TODO();

  print_asm_template1(not);
}
