#include "common.h"
#include "klib.h"
void init_mm(void);
void init_ramdisk(void);
void init_device(void);
void init_irq(void);
void init_fs(void);
void init_proc(void);

int main() {
#ifdef HAS_VME
  init_mm();
#endif

  Log("'Hello World!' from Nanos-lite");
  Log("Build time: %s, %s", __TIME__, __DATE__);

  init_ramdisk();

  init_device();

#ifdef HAS_CTE
  init_irq();
#endif
  Log("init_irq over");

  init_fs();
  Log("init_fs over");
  
  init_proc();
  Log("init_proc over");

#ifdef HAS_CTE
  _yield();
#endif

  panic("Should not reach here");
}
