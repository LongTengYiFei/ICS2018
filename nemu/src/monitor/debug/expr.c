#include "nemu.h"
#include "stdlib.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM,TK_REGISTER

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
  {"\\(",'('},          //left 
  {"\\)",')'},          //right 
  {"\\w+",TK_NUM},      //num 
  {"\\$\\w+",TK_REGISTER},
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
    printf("re[%d]:%d\n",i,re[i]);
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
        int pos = position;//save the position,because position will be changed later
        Log("Match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;//position change 

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
	case  '(':
	       strcpy(tokens[nr_token].str,"(");
	       tokens[nr_token].type = '(';
	       nr_token ++;
	       break;
	case  ')':
	       strcpy(tokens[nr_token].str,")");
	       tokens[nr_token].type = ')';
	       nr_token ++;
	       break;
	case  TK_NUM:
	       strncpy(tokens[nr_token].str,e + pos,substr_len);
	       tokens[nr_token].str[substr_len] = '\0';
	       tokens[nr_token].type = TK_NUM;
	       nr_token ++;
	       break;
	case  TK_REGISTER:
	       strncpy(tokens[nr_token].str,e + pos,substr_len);
	       tokens[nr_token].str[substr_len] = '\0';
	       tokens[nr_token].type = TK_REGISTER;
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

uint32_t valueOfToken(char *token){
   //register	
   if(strcmp(token,"$eax") == 0)
	   return cpu.eax;
   if(strcmp(token,"$ebx") == 0)
	   return cpu.ebx;
   if(strcmp(token,"$ecx") == 0)
	   return cpu.ecx;
   if(strcmp(token,"$edx") == 0)
	   return cpu.edx;
   if(strcmp(token,"$ebp") == 0)
	   return cpu.esp;
   if(strcmp(token,"$esp") == 0)
	   return cpu.esp;
   if(strcmp(token,"$esi") == 0)
	   return cpu.esi;
   if(strcmp(token,"$edi") == 0)
	   return cpu.edi;

   //0x....
   char *rest;
   if(token[0] == '0'&&token[1] == 'x')
       return strtol(token,&rest,16); 

   //regular number
   return atoi(token);
}

bool check_parentheses(int p,int q){
  if(tokens[p].type!='(')
	  return false;
  if(tokens[q].type!=')')
	  return false;
  char *parents;
  int nr_brackets = 0; 
  for(int i=0;i<=nr_token-1;i++)
	  if(tokens[i].type == '(')
		 nr_brackets++;
          else if(tokens[i].type == ')')
		 nr_brackets++;
  printf("nr_brackets is:%d\n",nr_brackets);
  if(nr_brackets == 1 || nr_brackets == 0)return false;
  parents = (char *)malloc(sizeof(char) * nr_brackets);
  char *check_stack = (char *)malloc(sizeof(char) * nr_brackets);
  int j=0;
  for(int i=0;i<=nr_token-1;i++)
	  if(tokens[i].type == '(')
		 parents[j++] = '(';
          else if(tokens[i].type == ')')
		 parents[j++] = ')';

  printf("parents is :%s\n",parents);
  if(parents[0] == '('&& parents[nr_brackets-1] == '(')
	  return false;
  if(parents[0] == ')'&& parents[nr_brackets-1] == ')')
	  return false;
  if(parents[0] == ')'&& parents[nr_brackets-1] == '(')
	  return false;
  if(nr_brackets%2 != 0)
	  return false;

  int rest_brackets=0;
  for(int i=1;i<=nr_brackets-2;i++){
     if(rest_brackets == 0){
       check_stack[rest_brackets] = parents[i];
       rest_brackets ++;
       continue;
     }
     else{
       check_stack[rest_brackets] = parents[i];
       rest_brackets++;
       if(check_stack[rest_brackets-2]=='('&&check_stack[rest_brackets-1]==')')
	       rest_brackets -=2;
     }
  }
  if(rest_brackets != 0)return false;
  else return true;
}

uint32_t select_main_pos(int p,int q){
 int main_pos;
 char * flags = (char *)malloc(nr_token * sizeof(char));
 //if the token is not op,it is false.
 for(int i=0;i<=nr_token-1;i++)
	 if(tokens[i].type=='+'||tokens[i].type=='-'||tokens[i].type=='*'||tokens[i].type=='/')
		 flags[i] = true;
         else flags[i] = false;
 
 //if the op is in the brackets,it is false
 for(int i=2;i<=nr_token-3;i++)
    if(flags[i] == true)
	if(tokens[i-2].type=='('&&tokens[i+2].type== ')')
	   flags[i]= false;

 char main_op = 0;
 int pre_pos = 0;
 //+ 43
 //- 45
 //* 42
 //div 47
 for(int i=0;i<=nr_token-1;i++)
 {
   if(main_op == 0 && flags[i] == true)
     { 
       main_op = tokens[i].type;
       pre_pos = i;
       continue;
     }

    if(main_op == '*' && flags[i] == true && tokens[i].type == '+')
    {
      main_op = '+';
      flags[pre_pos] = false;
      pre_pos = i;
      continue;
    } 
    if(main_op == '*' && flags[i] == true && tokens[i].type == '-')
    {
      main_op = '-';
      flags[pre_pos] = false;
      pre_pos = i;
      continue;
    } 
    if(main_op == '/' && flags[i] == true && tokens[i].type == '+')
    {
      main_op = '+';
      flags[pre_pos] = false;
      pre_pos = i;
      continue;
    } 
    if(main_op == '/' && flags[i] == true && tokens[i].type == '-')
    {
      main_op = '-';
      flags[pre_pos] = false;
      pre_pos = i;
      continue;
    }

    if(main_op == '+' && flags[i] == true && tokens[i].type == '*'){
      flags[i] = false;
      continue;
    }
    if(main_op == '+' && flags[i] == true && tokens[i].type == '/'){
      flags[i] = false;
      continue;
    }
    if(main_op == '-' && flags[i] == true && tokens[i].type == '*'){
      flags[i] = false;
      continue;
    }
    if(main_op == '-' && flags[i] == true && tokens[i].type == '/'){
      flags[i] = false;
      continue;
    }
//back trace to find the main_pos
    for(int i = nr_token -1;i>=0;i--)
	   if(flags[i] == true)
	   {
	    main_pos = i;
	    break;
	   } 
 }

 return main_pos;
}

uint32_t eval(int p,int q){
    if(p>q){
       printf("Bad expression.\n");
       return 0;
    }
    else if(p == q)
      return valueOfToken(tokens[p].str);
    else if(check_parentheses(p,q) == true)
      return eval(p+1,q-1);
    else{
    int op = select_main_pos(p,q);
    int val1 = eval(p,op - 1);
    int val2 = eval(op + 1,q);
    int op_type = tokens[op].type;
    switch(op_type){
	    case '+':return val1 + val2;
	    case '-':return val1 - val2;
	    case '*':return val1 * val2;
	    case '/':return val1 / val2;
            default : assert(0);
    }
   }
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

  printf("token[0]'s value :%x\n",valueOfToken(tokens[0].str));
  if(check_parentheses(0,nr_token-1) == true)printf("parents true\n");
  else printf("parents false\n");
  /* TODO: Insert codes to evaluate the expression. */
  //return eval(0,nr_token-1);
  printf("The main_pos is:%d\n",select_main_pos(0,nr_token-1));
  return 0;
}
