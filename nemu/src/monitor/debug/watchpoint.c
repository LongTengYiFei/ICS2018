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
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
int nr_free_wp = NR_WP;

WP* new_wp(char * express){
 if(nr_free_wp == 0){
   printf("There is no free watchpoint.");
   return 0;
 } 
 for(int i=0;i<=NR_WP -1;i++)
    if(wp_pool[i].expr == 0)
    {
	wp_pool[i].expr = express;
        nr_free_wp --;
	return &wp_pool[i];
    }

  
}

void free_wp(int N){

   for(int i=1;i<=NR_WP;i++)
      if(i == N)
      {
	 wp_pool[i-1].expr = 0;
	 nr_free_wp ++;
	 break;
      }
}
