#include <stdint.h>
#include <stdlib.h>
#define SYS_yield 1
#include <stdio.h>
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
  return _syscall_(SYS_yield, 0, 0, 0);
}
