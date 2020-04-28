#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  //printf("cc come in!\n");
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  //printf("subcode = 0x%x\n",subcode);
  //printf("subcode & 0xe = 0x%x\n",subcode & 0xe);
  switch (subcode & 0xe) {
    case CC_O:// 0 overflow
      *dest = cpu.eflags.OF;
      break;
    case CC_B:// 2 below
      *dest = cpu.eflags.CF;
      break;
    case CC_E://4 equal
      *dest = cpu.eflags.ZF;
      break;
    case CC_BE:// 6 below or equal
      //cf==1 or zf==1
      *dest = ((cpu.eflags.CF)||(cpu.eflags.ZF));
      break;
    case CC_S:// 8 sign
      *dest = cpu.eflags.SF;
      break;

    case CC_L://12 c less
      *dest = (cpu.eflags.SF != cpu.eflags.OF);
      break;
    case CC_LE://14 d less or equal
      *dest = ((cpu.eflags.ZF) || (cpu.eflags.SF != cpu.eflags.OF));
      break;
      //TODO();
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert != 0) {
    rtl_xori(dest, dest, 0x1);
  }
  //printf("cc over!\n");
}
