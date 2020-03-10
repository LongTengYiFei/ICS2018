#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  printf("do_event() begin! e.event = %d\n",e.event);
  switch (e.event) {
    case _EVENT_YIELD : printf("do_event : yield!!\n");
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
