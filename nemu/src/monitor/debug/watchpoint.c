#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "nemu.h"

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

WP* new_wp()
{
	WP *p=free_;
	if(p==NULL)//没有可用监视点
		assert(0);
	free_=free_->next;//删除free结点
	p->next=NULL;
	if(head)
	{
		p->next=head;
		head=p;
	}
	else
	{
		p->next=NULL;
		head=p;
	}
	return p;

}

void free_wp(WP *wp)
{
	WP *p=free_,*q=head;
	if(!p)
	{
		free_=wp;
		p=free_;
	}
	else
	{
		while(p->next)
			p=p->next;
		p->next=wp;
	}
	if(wp->NO==head->NO)
        	head=head->next;
    	else
    	{
    		while(q->next&&(q->next->NO!=wp->NO))
    			q=q->next;
        	if(q->next->NO==wp->NO)
        		q->next=q->next->next;
    	}
    	wp->next=NULL;
    	wp->expr[0]='\0';
    	wp->old_val=0;
}

int set_watchpoint(char *e){
	WP *p=new_wp();//新的监视点
	bool s=true;
	strcpy(p->expr,e);//存入表达式
	if(!strncmp(p->expr,"$eip == ",8)){
		printf("Set Break Point $eip == ADDR\n");
	}
	else{
	p->old_val=expr(p->expr,&s);//旧值
	if(!s){
		printf("Fail To Eval\n");
		return 0;
	}
	printf("Set watchpoint  #%d\n",p->NO);
	printf("expr=           %s\n",p->expr);
	printf("old_value=      %#x\n",p->old_val);
	}
	return 1;
}

bool delete_watchpoint(int NO){//直接用free_wp函数
	 WP *p=&wp_pool[NO];
	 free_wp(p);
	 printf("Watchpoint %d deleted\n",NO);
	 return true;
}

void info_w(WP *p){//一个简单的辅助函数
	 if(p->next!=NULL)
		 info_w(p->next);
	 printf(" %d\t%s\t0x%08x\n",p->NO,p->expr,p->old_val);
}

void list_watchpoint(){
	WP *p=head;
	if(p==NULL)
		printf("Have Not A Watchpoint!\n");
	else
		info_w(p);//调用函数
}

WP* scan_watchpoint(){
	WP *p=head;
	bool s=true;
	while(p)
	{
		p->new_val=expr(p->expr,&s);//计算新值
		if(!s)
			printf("Fail To Eval New_Val In WatchPoint %d\n",p->NO);
		else{
		if(p->new_val!=p->old_val)//如果值变化了
		{
			if(!strncmp(p->expr,"$eip == ",8)) {//是断点表达式
				printf("Hit break point,program paused\n");
				break;
			}
			else{
			printf("Hit watchpoint %d at address %#8x\n",p->NO,cpu.eip);
			printf("expr=         %s\n",p->expr);
			printf("old_value=     %#x\n",p->old_val);
			printf("new_value=     %#x\n",p->new_val);
			p->old_val=p->new_val;//更新
			printf("promgram paused\n");
			break;
			}
		}
		}
		p=p->next;
	}
	return p;
}
