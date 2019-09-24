#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  char expr[100];
  int value;
  struct watchpoint *next;
} WP;

#endif
