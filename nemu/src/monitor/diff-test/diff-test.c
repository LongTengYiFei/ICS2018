#include <dlfcn.h>

#include "nemu.h"
#include "monitor/monitor.h"
#include "diff-test.h"
struct {
   uint32_t base;
   uint16_t len;
}IDTR;

static void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n);
static void (*ref_difftest_getregs)(void *c);
static void (*ref_difftest_setregs)(const void *c);
static void (*ref_difftest_exec)(uint64_t n);

static bool is_skip_ref;
static bool is_skip_dut;
static bool is_skip_difftest;

#define PMEM_SIZE (128 * 1024 * 1024)
void difftest_skip_ref() { is_skip_ref = true; }
void difftest_skip_dut() { is_skip_dut = true; }
void difftest_off() { is_skip_difftest = true; }//off is skip
void difftest_on() { 
	is_skip_difftest = false;
        //printf("ENTRY_START = 0x%x\n",ENTRY_START);	
        ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
        ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), PMEM_SIZE - ENTRY_START);
        ref_difftest_setregs(&cpu);
        //idtr
        CPU_state ref_r;

	int ref_base = cpu.idtr.base;
	int ref_len = cpu.idtr.len;
        vaddr_write(0x7e00, ref_len, 2); 
        vaddr_write(0x7e00 + 2, ref_base, 4); 
        ref_difftest_memcpy_from_dut(0x7e00, guest_to_host(0x7e00), 6);
	//lidt
	//opcode 0f01
        vaddr_write(0x7e40, 0x010f, 2);
        vaddr_write(0x7e40 + 2,0x18, 1);//(eax)	
        ref_difftest_memcpy_from_dut(0x7e40, guest_to_host(0x7e40), 3);
	//exec(1)
	uint32_t pre_eip = cpu.eip;
	cpu.eip = 0x7e40;
        ref_difftest_setregs(&cpu);

	uint32_t pre_eax = cpu.eax;
	cpu.eax = 0x7e00;
        ref_difftest_setregs(&cpu); 

        ref_difftest_exec(1);

	cpu.eip = pre_eip;//restore
	cpu.eax = pre_eax;//restore
        ref_difftest_setregs(&cpu);
        return ;
}//on is not skip

void init_difftest(char *ref_so_file, long img_size) {
   #ifndef DIFF_TEST
     return;
   #endif

  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY | RTLD_DEEPBIND);
  assert(handle);

  ref_difftest_memcpy_from_dut = dlsym(handle, "difftest_memcpy_from_dut");
  assert(ref_difftest_memcpy_from_dut);

  ref_difftest_getregs = dlsym(handle, "difftest_getregs");
  assert(ref_difftest_getregs);

  ref_difftest_setregs = dlsym(handle, "difftest_setregs");
  assert(ref_difftest_setregs);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(void) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.", ref_so_file);

  ref_difftest_init();
  ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), img_size);
  ref_difftest_setregs(&cpu);
}
extern int foot = 0;
void difftest_step(uint32_t eip) {
  //printf("DiffTest_step Come in!!\n");

  if(is_skip_difftest == true)
	  return ;

  CPU_state ref_r;

  if (is_skip_dut) {
    is_skip_dut = false;
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_setregs(&cpu);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_getregs(&ref_r);


  // TODO: Check the registers state with the reference design.
  // Set `nemu_state` to `NEMU_ABORT` if they are not the same.
  foot++;
  if(cpu.eip == 0x101b6d){
      nemu_state = NEMU_STOP;
      printf("foot = %d\n",foot);
  }
 if(cpu.eax != ref_r.eax ||cpu.ecx != ref_r.ecx ||cpu.edx != ref_r.edx ||cpu.ebx != ref_r.ebx ||cpu.esp != ref_r.esp ||cpu.ebp != ref_r.ebp ||cpu.esi != ref_r.esi ||cpu.edi != ref_r.edi ||cpu.eip != ref_r.eip ){
  nemu_state = NEMU_ABORT;
  printf("foot = %d\n",foot);
  printf("cpu.eax = 0x%x ,ref_r.eax = 0x%x\n",cpu.eax, ref_r.eax);
  printf("cpu.ecx = 0x%x ,ref_r.ecx = 0x%x\n",cpu.ecx, ref_r.ecx);
  printf("cpu.edx = 0x%x ,ref_r.edx = 0x%x\n",cpu.edx, ref_r.edx);
  printf("cpu.ebx = 0x%x ,ref_r.ebx = 0x%x\n",cpu.ebx, ref_r.ebx);
  printf("cpu.esp = 0x%x ,ref_r.esp = 0x%x\n",cpu.esp, ref_r.esp);
  printf("cpu.ebp = 0x%x ,ref_r.ebp = 0x%x\n",cpu.ebp, ref_r.ebp);
  printf("cpu.esi = 0x%x ,ref_r.esi = 0x%x\n",cpu.esi, ref_r.esi);
  printf("cpu.edi = 0x%x ,ref_r.edi = 0x%x\n",cpu.edi, ref_r.edi);
  printf("cpu.eip = 0x%x ,ref_r.eip = 0x%x\n",cpu.eip, ref_r.eip);

 }
  //TODO();
}
