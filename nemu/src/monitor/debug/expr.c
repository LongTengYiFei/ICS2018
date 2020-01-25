#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-",'-'},          //sub 
  {"\\*",'*'},          //multi 
  {"\\/",'/'},          //div 
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    printf("re[%d]:%c\n",i,re[i]);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
  
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;
static bool make_token(char *e) {
  int position = 0;
  int i;
  
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("Match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
        case  '+':
	       strcpy(tokens[nr_token].str,"+");
	       tokens[nr_token].type = '+';
	       nr_token ++;
	       break;
        case '-':
	       strcpy(tokens[nr_token].str,"-");
	       tokens[nr_token].type = '-';
	       nr_token ++;
	       break;
	case  '*':
	       strcpy(tokens[nr_token].str,"*");
	       tokens[nr_token].type = '*';
	       nr_token ++;
	       break;
	case  '/':
	       strcpy(tokens[nr_token].str,"/");
	       tokens[nr_token].type = '/';
	       nr_token ++;
	       break;
        case TK_EQ:
	       strcpy(tokens[nr_token].str,"==");
	       tokens[nr_token].type = TK_EQ;
	       nr_token ++;
	       break;
	case TK_NOTYPE:
	       strcpy(tokens[nr_token].str," ");
	       tokens[nr_token].type = TK_NOTYPE;
	       nr_token ++;
	       break;



          default: ;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
   
  if (!make_token(e)) {
    *success = false;
    return 0;
   
  }
  printf("nr_token = %d\n" , nr_token);

  for(int i = 0; i <= nr_token-1;i++){
     printf("tokens[%d].type : %d  ",i,tokens[i].type);
     printf("tokens[%d].str : %s\n",i,tokens[i].str);
  }
  /* TODO: Insert codes to evaluate the expression. */

  return 0;
}
