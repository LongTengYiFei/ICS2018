#include <am.h>
#include <x86.h>
#include <amdev.h>
//0x60 port
//breakcode is keycode  ---release
//makecode is keycode | 0x8000  -----push
//if push keydown is 1
//if release keydown is 0
size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      uint32_t p = inl(0x60);
      kbd->keydown = ((p & 0x8000) == 0) ? 0: 1;
      kbd->keycode = p;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
