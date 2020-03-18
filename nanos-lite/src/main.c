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
  printf("init_irq over\n\n");

  init_fs();
  printf("init_fs over\n\n");

  init_proc();
  printf("init_proc over\n\n");

#ifdef HAS_CTE
  //printf("pending yeild\n");
  _yield();
#endif

  panic("Should not reach here");
}
