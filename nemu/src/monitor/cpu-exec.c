#include "nemu.h"
#include "monitor/monitor.h"
#include"string.h"
#include "monitor/watchpoint.h"
/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

int nemu_state = NEMU_STOP;

void exec_wrapper(bool);

static uint64_t g_nr_guest_instr = 0;

void nr_guest_instr_add(uint32_t n) {
  g_nr_guest_instr += n;
}

void monitor_statistic() {
  Log("total guest instructions = %ld", g_nr_guest_instr);
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  if (nemu_state == NEMU_END || nemu_state == NEMU_ABORT) {
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  }
  nemu_state = NEMU_RUNNING;

  bool print_flag = n < MAX_INSTR_TO_PRINT;

   //main loop
   int * pre_value = (int *)malloc(get_nr_wp() * sizeof(int));
   int * pos_value = (int *)malloc(get_nr_wp() * sizeof(int));
    for(int i=0; i<=get_nr_wp()-1 ;i++)
	    if(expresses[i] != 0)
	    {
		    pre_value[i] = expr(expresses[i]);
		    printf("pre_value[%d] is %d\n",i,pre_value[i]);
	    }
            else 
		    pre_value[i] = 0;//maybe not good
  for (; n > 0; n --) {

    /* Execute one instruction, including
     * instruction fetch,
     * instruction decode,
     * actual execution. */
    exec_wrapper(print_flag);
    nr_guest_instr_add(1);
  
#ifdef DEBUG
    /* TODO: check watchpoints here. */

    for(int i=0;i<=get_nr_wp()-1;i++)
	    if( expresses[i] != 0 )
	    {
		    pos_value[i] = expr(expresses[i]);
		    printf("pos_value[%d] is %d\n",i,pos_value[i]);
	    }
            else 
		    pos_value[i] = 0;//maybe not good

    bool wp_trigger = false;
    for(int i=0;i<= get_nr_wp()-1;i++){
       if(expresses[i] != 0 && pre_value[i] != pos_value[i])
         {
	    wp_trigger = true;	 
            printf("wp NO.%d has been triggered!\n",i +1);
         }
    }
    if(wp_trigger == true)
      nemu_state = NEMU_STOP;
      
#endif

#ifdef HAS_IOE
    extern void device_update();
    device_update();
#endif

    if (nemu_state != NEMU_RUNNING) {
      if (nemu_state == NEMU_END) {
        printflog("\33[1;31mnemu: HIT %s TRAP\33[0m at eip = 0x%08x\n\n",
            (cpu.eax == 0 ? "GOOD" : "BAD"), cpu.eip - 1);
        monitor_statistic();
        return;
      }
      else if (nemu_state == NEMU_ABORT) {
        printflog("\33[1;31mnemu: ABORT\33[0m at eip = 0x%08x\n\n", cpu.eip);
        return;
      }
    }
  }
  if(cpu.eip == 0x101b6d){nemu_state = NEMU_STOP;} 
  if (nemu_state == NEMU_RUNNING) { nemu_state = NEMU_STOP; }
}
