#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(cyf nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);


//My Commands Start-------------------------
static int cmd_si(char *args){
  //printf("si execute\n");
  //printf("args %s execute\n",args);
  int step_count = atoi(args);
  //printf("step_count is %d\n",step_count);

 //we need check the step_count's validation later
   cpu_exec(step_count); 
 return 0;
}
static int cmd_info(char *args){
  //printf("info excute\n");
  //printf("args %s execute\n",args);
  if(args != NULL){
    if(strcmp(args,"r") == 0){
	  printf("print register's value.\n");
          printf("EAX = %x\n",cpu.eax);
          printf("EDX = %x\n",cpu.edx);
          printf("ECX = %x\n",cpu.ecx);
          printf("EBX = %x\n",cpu.ebx);
          printf("EBP = %x\n",cpu.ebp);
          printf("ESI = %x\n",cpu.esi);
          printf("EDI = %x\n",cpu.edi);
          printf("ESP = %x\n",cpu.esp);
 /*
          printf("SF = %x\n",cpu.eflags.SF);
          printf("ZF = %x\n",cpu.eflags.ZF);*/
    }
    if(strcmp(args,"w") == 0){
	  printf("print watchpoint's information.\n");
	  printf("The NR_WP is %d\n",get_nr_wp());
	 
          for(int i=0;i<=get_nr_wp()-1;i++)
		 if(expresses[i]!=0)
			printf("WP NO. %d is :%s\n" ,i +1,expresses[i]); 
    
    }
  }
  return 0;
}

static int cmd_x(char *args){
  //printf("x execute\n");
  //printf("args \"%s\" excute \n",args);
  char * arg0 = strtok(args," ");
  char * arg1 = strtok(NULL," "); 
  //printf("arg0 \"%s\" excute \n",arg0);
  //printf("arg1 \"%s\" excute \n",arg1);

  int  N = atoi(arg0);//N
  char *rest;
  //int  n_addr = strtol(arg1,&rest,16);//address
  int n_addr = expr(arg1,true);
  printf("n_addr is :%x\n",n_addr);
  for(int i=1 ;i<=N;i++){
   printf("%x:",n_addr);
   printf("%x ", vaddr_read(n_addr,1));
   printf("%x ", vaddr_read(++n_addr,1));
   printf("%x ", vaddr_read(++n_addr,1));
   printf("%x\n", vaddr_read(++n_addr,1));
   n_addr++;
  }
  return 0;
}

static int cmd_p(char *args){// express
  printf("Express is : \"%s\"\n",args);
  int result = expr(args,true);
  printf("result 10 is:%d\n",result); 
  printf("result 16 is:%x\n",result); 

return 0;
}

static int  cmd_w(char *args){//new a watchpoint
  new_wp(args);
return 0;
}

static int  cmd_d(char *args){//delete a watchpoint
   int N =atoi(args);
   free_wp(N);
return 0;
}
static int  cmd_detach(char *args){
	return 0;
}
static int  cmd_attach(char *args){
	return 0;
}
//My Commands End------------------------------------
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  { "si","single instruction",cmd_si},
  {"info","information of register or watchpoint",cmd_info},
  {"x","scan memmory",cmd_x},
  {"p","caculate the value of express",cmd_p},
  {"w","new a watchpoint",cmd_w},
  {"d","delete a watchpoint",cmd_d},
  {"detach","exit diffTest",cmd_detach},
  {"attach","come into diffTest",cmd_attach}
  /* TODO: Add more commands */

};
#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  //wo gai le zhe ge no shi
  //is_batch_mode = false;
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);
    
    //printf("str is %s\n",str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }
    
    //printf("cmd is %s\n",cmd);
    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

    //printf("args is %s\n",args);
#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    //search command one by one
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
