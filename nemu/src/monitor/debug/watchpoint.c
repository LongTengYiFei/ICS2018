#include "monitor/watchpoint.h"
#include "monitor/expr.h"

//change the number as you want
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].expr = 0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
int nr_free_wp = NR_WP;


WP* new_wp(char * express){
 if(nr_free_wp == 0){
   printf("There is no free watchpoint.\n");
   return 0;
 } 
 for(int i=0;i<=NR_WP -1;i++)
    if(wp_pool[i].expr == 0)
    {
	
	wp_pool[i].expr = (char *)malloc((strlen(express)+1) * sizeof(char));
	strcpy(wp_pool[i].expr,express);
	wp_pool[i].expr[strlen(express)] = '\0';
	printf("The new address is %x\n",wp_pool[i].expr);
	printf("wp_pool[%d].expr is :%s\n",i,wp_pool[i].expr);
        nr_free_wp --;
	printf("The new wp is NO.%d\n",i+1);
	return &wp_pool[i];
    }

  
}

void free_wp(int N){
   int i=1;
   for(;i<=NR_WP;i++)
      if(i == N && wp_pool[i-1].expr != 0)
      {
	 wp_pool[i-1].expr = 0;
	 nr_free_wp ++;
	 printf("wp [%d] was deleted\n",N);
	 break;
      }
   if(i == NR_WP+1)
	printf("There is no [%d] wp\n",N);
   
}

char * expresses[32] = {0};


char * get_expr(int index){
   expresses[index] = (char *)malloc((strlen(wp_pool[index].expr)+1) * sizeof(char));
   strcpy(expresses[index],wp_pool[index].expr);
   
   printf("expr has been got:%s\n",expresses[index]);
   printf("address has been got:%x\n",expresses[index]);
   return expresses[index];
}
uint32_t get_nr_wp(){
  return NR_WP;
}
