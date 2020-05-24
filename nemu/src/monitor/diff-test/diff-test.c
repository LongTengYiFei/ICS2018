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
long memsize;

#define PMEM_SIZE (128 * 1024 * 1024)
void difftest_skip_ref() { is_skip_ref = true; }
void difftest_skip_dut() { is_skip_dut = true; }
void difftest_off() { is_skip_difftest = true; }//off is skip
void difftest_on() {
        printf("difftest on come in\n");	
	is_skip_difftest = false;
        //printf("ENTRY_START = 0x%x\n",ENTRY_START);	
        ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
        ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), memsize);
	ref_difftest_setregs(&cpu);
        //idtr
        CPU_state ref_r;
	uint32_t ref_base = cpu.idtr.base;
	uint16_t ref_len = cpu.idtr.len;
	
	printf("len = 0x%x\n",ref_len);
	printf("base = 0x%x\n",ref_base);
	
        vaddr_write(0x7e00, ref_len, 2); 
        vaddr_write(0x7e00 + 2, ref_base, 4); 

	int len_low = vaddr_read(0x7e00, 1);
	int len_high = vaddr_read(0x7e01, 1);
	/*
	printf("low = 0x%x\n",len_low);
	printf("high = 0x%x\n",len_high);
        */
	/*
	int base_first_byte = vaddr_read(0x7e00 +2,1);
	printf("base_first_byte= 0x%x\n",base_first_byte);
        */
        ref_difftest_memcpy_from_dut(0x7e00, guest_to_host(0x7e00), 6);
	//my instr---------------
	//opcode 0f01 lidt
	//eax 0x18
        vaddr_write(0x7e40, 0x0f, 1);
        vaddr_write(0x7e40 +1, 0x01, 1);
        vaddr_write(0x7e40 +2,0x18, 1);
        ref_difftest_memcpy_from_dut(0x7e40, guest_to_host(0x7e40), 3);
	//exec(1)
	uint32_t pre_eip = cpu.eip;
	cpu.eip = 0x7e40;
	uint32_t pre_eax = cpu.eax;
	cpu.eax = 0x7e00;

        ref_difftest_setregs(&cpu); 
        ref_difftest_exec(1);
        //check
	ref_difftest_getregs(&ref_r);
        printf("my instr has been executed, ref_r.eax = 0x%x\n",ref_r.eax);


	cpu.eip = pre_eip;//restore
	cpu.eax = pre_eax;//restore
        ref_difftest_setregs(&cpu);
        /*
        ref_difftest_getregs(&ref_r);
	printf("ref_r.eflags =0x%x\n",ref_r.eflags);
	printf("ref_r.cs =0x%x\n",ref_r.cs);
	printf("cpu.eflags =0x%x\n",cpu.eflags.value);
	printf("cpu.cs =0x%x\n",cpu.cs);
	*/
        return ;
}//on is not skip
void difftest_on_v2(){
    ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
    ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), memsize);
    ref_difftest_memcpy_from_dut(0x7e00, &cpu.idtr.len, 2);
    ref_difftest_memcpy_from_dut(0x7e02, &cpu.idtr.base, 4);
    uint8_t inst[] = {0x0f,0x01,0x1D,0x00,0x7e,0x00,0x00};
    ref_difftest_memcpy_from_dut(0x7e40, inst, sizeof(inst));
    CPU_state tmp = cpu;
    tmp.eip = 0x7e40;
    ref_difftest_setregs(&tmp);
    ref_difftest_exec(1);

    ref_difftest_setregs(&cpu);
}
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
  memsize = img_size;
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
  
  ref_difftest_getregs(&ref_r);
  uint32_t pre_ref_eax = ref_r.eax;
  uint32_t pre_ref_ecx = ref_r.ecx;
  uint32_t pre_ref_edx = ref_r.edx;
  uint32_t pre_ref_ebx = ref_r.ebx;
  uint32_t pre_ref_esp = ref_r.esp;
  uint32_t pre_ref_ebp = ref_r.ebp;
  uint32_t pre_ref_esi = ref_r.esi;
  uint32_t pre_ref_edi = ref_r.edi;
  uint32_t pre_ref_eip = ref_r.eip;
  uint8_t pre_ref_CF = ref_r.eflags.CF;
  uint8_t pre_ref_OF = ref_r.eflags.OF;
  uint8_t pre_ref_SF = ref_r.eflags.SF;
  uint8_t pre_ref_ZF = ref_r.eflags.ZF;


  ref_difftest_exec(1);
  printf("ref_has_executed\n");
  ref_difftest_getregs(&ref_r);


  // TODO: Check the registers state with the reference design.
  // Set `nemu_state` to `NEMU_ABORT` if they are not the same.
  foot++;
  if(cpu.eax != ref_r.eax)
  	nemu_state = NEMU_ABORT;
  if(cpu.ecx != ref_r.ecx)
  	nemu_state = NEMU_ABORT;
  if(cpu.edx != ref_r.edx)
  	nemu_state = NEMU_ABORT;
  if(cpu.ebx != ref_r.ebx)
  	nemu_state = NEMU_ABORT;
  if(cpu.esp != ref_r.esp)
  	nemu_state = NEMU_ABORT;
  if(cpu.ebp != ref_r.ebp)
  	nemu_state = NEMU_ABORT;
  if(cpu.esi != ref_r.esi)
  	nemu_state = NEMU_ABORT;
  if(cpu.edi != ref_r.edi)
  	nemu_state = NEMU_ABORT;
  if(cpu.eip != ref_r.eip)
  	nemu_state = NEMU_ABORT;
  
  if(nemu_state == NEMU_ABORT){
    printf("foot = %d\n",foot);
    printf("pre_ref_eax = 0x%x\n",pre_ref_eax);
    printf("pre_ref_ecx = 0x%x\n",pre_ref_ecx);
    printf("pre_ref_edx = 0x%x\n",pre_ref_edx);
    printf("pre_ref_ebx = 0x%x\n",pre_ref_ebx);
    printf("pre_ref_esp = 0x%x\n",pre_ref_esp);
    printf("pre_ref_ebp = 0x%x\n",pre_ref_ebp);
    printf("pre_ref_esi = 0x%x\n",pre_ref_esi);
    printf("pre_ref_edi = 0x%x\n",pre_ref_edi);
    printf("pre_ref_eip = 0x%x\n",pre_ref_eip);
    printf("----------------------------\n");
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
}
