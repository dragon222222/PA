#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include<assert.h>//assert函数

enum {
  TK_NOTYPE = 256, TK_EQ,TK_UEQ,TK_ST,TK_TEN,TK_REG,TK_IAC,//IAC解引英文缩写
  TK_BIG_OR_EQ,TK_SMA_OR_EQ,TK_MOVL,TK_MOVR,

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
  {"\\-", '-'},         //减号
  {"\\*", '*'},         //乘号
  {"\\/", '/'},         //除号
  {"==",TK_EQ},      //equal
  {"!=", TK_UEQ},        //不相等
  {"&&", '&'},      //与
  {"\\|\\|", '|'},    //或
  {"!", '!'},    //非
  {"0x[0-9a-f]+",TK_ST},   //十六进制
  {"[0-9]+",TK_TEN},    //十进制
  {"\\$[a-ehilpx]{2,3}", TK_REG},  //寄存器
  {"\\(", '('},     //左括号
  {"\\)", ')'},    //右括号
  {">=",TK_BIG_OR_EQ},
  {"<=",TK_SMA_OR_EQ},
  {"<<",TK_MOVL},
  {">>",TK_MOVR},
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
        switch (rules[i].token_type) {
		case TK_NOTYPE: break;
                default:{ tokens[nr_token].type = rules[i].token_type;
			  strncpy (tokens[nr_token].str,substr_start,substr_len);//这里我用了strncpy函数，strncpy是为拷贝字符而生的，而strcpy是拷贝字符串而生的,我这里需要的是“字符”.
			  nr_token ++;
			}
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

bool check_parentheses(int p,int q){
  int left=0;
  int right=0;
  if(!(tokens[p].type=='('&&tokens[q].type==')'))
          return false;
  for(p=p+1;p<q;p++){
        if(tokens[p].type=='(')
                left++;
        if(tokens[p].type==')')
                right++;
        if(right>left)
                return false;//任何时刻右括号都不会多于左括号
  }
  if(right!=left)
          return false;
  return true;
}

uint32_t find_dominated_op(int p,int q){
  /*对于这个函数，我有点难以下手，一看就感觉很复杂。好在我看了讲义，上面帮我分析了好多*/
  int x=p;
  int sum=0;//这个是来保证中心运算符在括号外面
  for(;p<q;p++){
    if(tokens[p].type=='(')
            sum++;
    if(tokens[p].type==')')
            sum--;
    if(sum==0){
      if(tokens[p].type==TK_EQ||tokens[p].type==TK_UEQ||tokens[p].type=='&'||tokens[p].type=='|')
              return p;//这些运算符肯定是
      if(tokens[p].type==TK_IAC)
              continue;//指针解引用直接“放弃”
      if(tokens[p].type=='+')
              x=p;//虽然是最低的，但是要根据结合性
      if(tokens[p].type=='-')
              if(tokens[p-1].type!='-'||tokens[p-1].type!='+'||tokens[p-1].type!='*'||tokens[p-1].type!='/')
                      x=p;//后面要求加入负号，所以判断一下
      if(tokens[p].type=='*'||tokens[p].type=='/')
              if(tokens[x].type!='+'||tokens[x].type!='-')
                      x=p;//如果没有比乘除级别低的，就选乘除,有点矮子头上选将军的感觉
    }
  }
  return x;//此时x就是结合了运算级别以及结合性的结果
}

uint32_t eval(int p,int q){
  if(p > q) {
    return false;
  }
  else if(p == q) {//单个表达式，只能为数字或者寄存器的情况
    int num= 0;
    if(tokens[p].type == TK_ST)//十六进制转化成十六进制数
          sscanf(tokens[p].str, "%x",&num);
    else if(tokens[p].type == TK_TEN)//十进制数转化成十进制数
          sscanf(tokens[p].str, "%d",&num);
    else if(tokens[p].type == TK_REG) {//是寄存器
      for(int i = 0;i < 4;i ++)
        tokens[p].str[i] = tokens[p].str[i + 1];//把$号去掉
      if(strcmp(tokens[p].str,"eip") == 0) //如果是eip寄存器
        num= cpu.eip;
      else{
          for(int i = 0;i < 8;i ++){
            if(strcmp(tokens[p].str,regsl[i]) == 0) {//规定只有32位寄存器
              num=cpu.gpr[i]._32;
              break;
            }
	  }
	}
    }
    return num;
  }
  else if(check_parentheses(p, q) == true) {
    return eval(p + 1, q - 1);
  }
  else{
    int op,val1,val2;
    op= find_dominated_op(p,q);
    if(op==p&&tokens[p].type==TK_IAC)//指针解引用情况
            return vaddr_read(eval(p+1,q),4);//此时的地址
    if(op==p&&tokens[p].type=='-')//负数表达式取负
            return -eval(op+1,q);
    if(op==p&&tokens[p].type=='!')//取非
            return !eval(op+1,q);
    val1= eval(p,op-1);
    val2= eval(op+1, q);
    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1- val2;
      case '*': return val1* val2;
      case '/': return val1/ val2;
      case TK_EQ: return val1 == val2;
      case TK_UEQ: return val1!= val2;
      case '&': return val1& val2;
      case '|': return val1|val2;
      case TK_BIG_OR_EQ:return val1>=val2;
      case TK_SMA_OR_EQ:return val1<=val2;
      case TK_MOVL:return val1<<val2;
      case TK_MOVR:return val1>>val2; 	
      default: assert(0);
    }
  }
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  for(int i=0;i<nr_token;i++)
          if(tokens[i].type=='*'&&(i==0||tokens[i-1].type=='+'||tokens[i-1].type=='-'||tokens[i-1].type=='*'||tokens[i-1].type=='/'))
                  tokens[i].type=TK_IAC;
  return eval(0,nr_token-1);
}
