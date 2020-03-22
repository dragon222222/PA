#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
typedef unsigned char *byte_pointer;//辅助cmd_x函数打印字节顺序

  
 
       
                                       
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

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
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Single step execution",cmd_si },
  { "info","Print register",cmd_info},
  { "x","Scan memory",cmd_x},

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

static int cmd_si(char *args){
  int i=0;//指令数目
  char *arg=strtok(NULL," ");/*这个是看上面的函数的，之后我百度了一下，发现
  这个strtok函数的用处，先用NULL来承接命令，为后面的sscanf函数准备*/
  if(!arg)
	cpu_exec(1);//si与si 1是等价的
  else{
  	sscanf(arg,"%d",&i);/*sscanf函数是来传递数字给i的，也就是执行多少次指令*/
	if(i<=0)
		cpu_exec(-1);//与cmd_c一样
	else
		cpu_exec(i);
  }
  return 0;
}

static int cmd_info(char *args){
  int m=0,n;
  char *arg=strtok(NULL," ");//与之前的函数类似，以此判断要执行的指令
  if(strcmp(arg,"r")==0){
  	for(;m<8;m++)
		printf("%s:       %8x\n",regsl[m],cpu.gpr[m]._32);
	for(m=0;m<8;m++)
                printf("%s:       %8x\n",regsw[m],cpu.gpr[m]._16);
	for(m=0;m<8;m++){
		for(n=0;n<2;n++)
			 printf("%s:       %8x\n",regsb[m],cpu.gpr[m]._8[n]);
	}
  /*三种寄存器，32、16以及8位，都是8个，其中8位的有两个维度的*/
  }
  else if(strcmp(arg,"w")==0)
	  printf("Not started yet! Forgive me!\n");
  return 0;
}

void show_bytes(byte_pointer start, int len) {//辅助函数，打印字节顺序
     int i;
     for (i = 0; i < len; i++)
     printf("%.2x ", start[i]);   
     printf("\n");
}

static int cmd_x(char *args){
  int i,m,n; //m是输出的行数，n是相应地址的数据
  vaddr_t addr;//由memory.c中可知vaddr的相关函数以及定义
  char *arg1=strtok(NULL," ");
  char *arg2=strtok(NULL," ");
  sscanf(arg1,"%d",&m);
  sscanf(arg2,"%x",&addr);
  printf("Address    Dword block    Byte sequence\n");
  for(i=0;i<m;i++){
  	printf("%#x    ",addr);
	n=vaddr_read(addr,4);
	printf("%#x      ",n);
	show_bytes((byte_pointer)&n,sizeof(int));//调用函数
	addr+=4;//每次加4
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
