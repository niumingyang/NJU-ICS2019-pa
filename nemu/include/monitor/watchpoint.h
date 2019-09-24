#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#define WP_LEN 100

typedef struct watchpoint {
  int NO;
  char expr[WP_LEN];
  int value;
  struct watchpoint *next;
} WP;

#endif
