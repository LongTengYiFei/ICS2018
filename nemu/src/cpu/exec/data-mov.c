#include "cpu/exec.h"

make_EHelper(mov) {

  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
 //printf("Come into EHelper(push)!\n");
 //printf("id_dest->width=%d\n",id_dest->width);
  if(id_dest->width == 1)
  {
    uint8_t utmp = id_dest->val;
    int8_t tmp = utmp;
    id_dest->val = tmp;

    rtl_push(&id_dest->val);
  }
  else rtl_push(&id_dest->val);

  print_asm_template1(push);
  //printf("EHelper(push) over\n");
}

make_EHelper(pop) {
  //printf("Come into EHelper(pop)!\n");
  rtl_pop(&t0);
  
  if(id_dest->width == 1){
     uint8_t utmp = t0;
     int8_t tmp = utmp;
     id_dest->val = tmp;  
  
  }else 
     id_dest->val = t0;


  //printf("id_dest->reg = %d\n",id_dest->reg);//id_dest->reg is which reg
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
  //printf("EHelper(pop) over!\n");
}

make_EHelper(pusha) {
  printf("come into pusha!\n");
  t0 = cpu.esp;
  printf("cpu.esp = 0x%x\n",cpu.esp);
  printf("cpu.eax = 0x%x\n",cpu.eax);
  printf("cpu.ecx = 0x%x\n",cpu.ecx);
  printf("cpu.edx = 0x%x\n",cpu.edx);
  printf("cpu.ebx = 0x%x\n",cpu.ebx);
  printf("cpu.ebp = 0x%x\n",cpu.ebp);
  printf("cpu.esi = 0x%x\n",cpu.esi);
  printf("cpu.edi = 0x%x\n",cpu.edi);
  t1 = cpu.eax;
  rtl_push(&t1);
  t1 = cpu.ecx;
  rtl_push(&t1);
  t1 = cpu.edx;
  rtl_push(&t1);
  t1 = cpu.ebx;
  rtl_push(&t1);

  rtl_push(&t0);

  t1 = cpu.ebp;
  rtl_push(&t1);
  t1 = cpu.esi;
  rtl_push(&t1);
  t1 = cpu.edi;
  rtl_push(&t1);
//  TODO();

  printf("pusha over!\n");
  printf("\n");
  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&cpu.edi);	
  rtl_pop(&cpu.esi);	
  rtl_pop(&cpu.ebp);	
  rtl_pop(&t0);//dont need esp	
  rtl_pop(&cpu.ebx);	
  rtl_pop(&cpu.edx);	
  rtl_pop(&cpu.ecx);	
  rtl_pop(&cpu.eax);	
  //TODO();

  print_asm("popa");
}

make_EHelper(leave) {/*
  printf("leave begin!\n");
  printf("EBP = 0x%x ",cpu.ebp);
  printf("ESP = 0x%x\n",cpu.esp);
  printf("i default the size is 32\n");
  */
  rtl_mv(&cpu.esp, &cpu.ebp);
  //printf("EBP = 0x%x\n",cpu.ebp);
  //printf("pop ebp\n");
  rtl_pop(&cpu.ebp);
  //printf("EBP = 0x%x\n",cpu.ebp);
  //printf("leave over!\n");
  //-----------------------------------------
  /*
  reg_l(4) = reg_l(5);
  rtl_pop(&t0);
  reg_w(5) = t0;*/
  print_asm("leave");
}

make_EHelper(cltd) {
 /* printf("cltd come in !\n");
  printf("cpu.eax = 0x%x\n",cpu.eax);
  printf("AX = 0x%x\n",cpu.gpr[R_EAX]._16);*/
	
  if (decoding.is_operand_size_16) {
        rtl_sext(&t0, &reg_l(R_EAX),2);
	rtl_shri(&reg_l(R_EDX),&t0, 16);
//    TODO();
  }
  else {
    rtl_sari(&reg_l(R_EDX),&reg_l(R_EAX),31);
    //TODO();
  }
  //printf("cltd over !\n");

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
	  rtl_shli(&reg_l(R_EAX), &reg_l(R_EAX), 24);
		rtl_sari(&reg_l(R_EAX), &reg_l(R_EAX), 8);
	rtl_shri(&reg_l(R_EAX), &reg_l(R_EAX), 16);
    //TODO();
  }
  else {
	  rtl_sext(&reg_l(R_EAX), &reg_l(R_EAX), 2);
    //TODO();
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
  //printf("lea come in\n");
  //printf("id_dest->type = %d\n",id_dest->type);
  //printf("id_src->type = %d\n",id_src->type);
  //printf("id_src->addr = 0x%x\n",id_src->addr);
  operand_write(id_dest, &id_src->addr);
  //printf("lea over\n");
  print_asm_template2(lea);
}
