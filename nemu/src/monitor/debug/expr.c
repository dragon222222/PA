#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ=1,TK_UEQ=0,TK_ST=16,TK_TEN=10,TK_REG=225,

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
  {"==", TK_EQ},         // equal
  {"\\-", '-'},         //减号
  {"\\*", '*'},         //乘号
  {"\\/", '/'},         //除号
  {"!=", TK_UEQ},        //不相等
  {"&&", '&'},      //与
  {"\\|\\|", '|'},    //或
  {"!", '!'},    //非
  {"0x[0-9a-f]+",TK_ST},   //十六进制
  {"[0-9]+",TK_TEN},    //十进制
  {"\\s[a-ehilpx]{2,3}", TK_REG},  //寄存器
  {"\\(", '('},     //左括号
  {"\\)", ')'},    //右括号
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
int nr_token;//全局变量，看仔细一点呀大兄弟

void Auxiliary(int i,int j,char *a){    //辅助函数，用来帮助make_token函数
  tokens[nr_token].type = rules[i].token_type;//这里rule在开头
  strncpy(tokens[nr_token].str,a,j);/*这里我用了strncpy函数，strncpy是为拷贝字符而生的，而strcpy是拷贝字符串而生的,我这里需要的是“字符”.*/
}


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

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if(rules[i].token_type==TK_NOTYPE) continue;
        switch (rules[i].token_type) {
		case '+':{
				 Auxiliary(i,substr_len,substr_start);
				 nr_token++;
			 }break;
 		case '-':{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case '*':{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case '/':{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case TK_EQ:{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case TK_UEQ:{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case TK_ST:{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case TK_TEN:{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case '&':{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case '|':{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case '!':{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case '(':{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
		case ')':{
                                 Auxiliary(i,substr_len,substr_start);
                                 nr_token++;
                         }break;
          default:assert(0);//出错
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

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
