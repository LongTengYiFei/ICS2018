#include <am.h>
#include <x86.h>
#include <klib.h>
static _Context* (*user_handler)(_Event, _Context*) = NULL;

void vectrap();
void vecnull();

_Context* irq_handle(_Context *tf) {
  _Context *next = tf;
    printf("tf->irq = 0x%x\n",tf->irq);
  if (user_handler) {
    _Event ev = {0};/*
    printf("come into irq_handle!\n");
    printf("tf->edi = 0x%x\n",tf->edi);
    printf("tf->esi = 0x%x\n",tf->esi);
    printf("tf->ebp = 0x%x\n",tf->ebp);
    printf("tf->esp = 0x%x\n",tf->esp);
    printf("tf->ebx = 0x%x\n",tf->ebx);
    printf("tf->edx = 0x%x\n",tf->edx);
    printf("tf->ecx = 0x%x\n",tf->ecx);
    printf("tf->eax = 0x%x\n",tf->eax);

    printf("sizeof(tf->irq) = 0x%d\n",sizeof(tf->irq));
    printf("tf->irq = 0x%x\n",tf->irq);
    printf("tf->irq = d %d\n",tf->irq);
    printf("tf->irq = c %c\n",tf->irq);
    printf("tf->err = 0x%x\n",tf->err);

    printf("tf->eip = 0x%x\n",tf->eip);
    printf("tf->cs = 0x%x\n",tf->cs);
    printf("tf->eflags = 0x%x\n",tf->eflags);
    printf("-------------------------\n");*/
    printf("sizeof(tf->irq) = 0x%d\n",sizeof(tf->irq));
    printf("tf->irq = 0x%x\n",tf->irq);
    switch (tf->irq) {
      case 0x20: ev.event = _EVENT_IRQ_TIMER; break;
      case 0x80: ev.event = _EVENT_SYSCALL; break;
      case 0x81: ev.event = _EVENT_YIELD; break;
      default: ev.event = _EVENT_ERROR; break;// NO 1
    }

    next = user_handler(ev, tf);
    if (next == NULL) {
      next = tf;
    }
  }

  printf("irq_handle over\n");
  return next;
}

static GateDesc idt[NR_IRQ];

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  return NULL;
}

void _yield() {
  //printf("come into _yield\n");
  asm volatile("int $0x81");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
