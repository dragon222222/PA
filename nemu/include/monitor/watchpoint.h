#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[32];
  int new_val;
  int old_val;

} WP;

WP* new_wp();
void free_wp(WP *wp);
int set_watchpoint(char *e);
bool delete_watchpoint(int NO);
void info_w(WP *p);
void list_watchpoint(void);
WP* scan_watchpoint(void);

#endif
