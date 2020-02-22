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
    case CC_O:
    case CC_B:
    case CC_E:
    case CC_BE:// 6
      //cf==1 or zf==1
      *dest = ((cpu.eflags.CF)||(cpu.eflags.ZF));
      break;
    case CC_S:
    case CC_L:
    case CC_LE:
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
