#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
//control.c
make_EHelper(call);
make_EHelper(ret);
make_EHelper(jmp);
make_EHelper(jcc);


//data-mov.c
make_EHelper(push);
make_EHelper(pop);
make_EHelper(lea);
make_EHelper(leave);

//arith.c
make_EHelper(sub);
make_EHelper(cmp);
make_EHelper(add);


//logic.c
make_EHelper(and);

