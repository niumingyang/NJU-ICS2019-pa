#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case _EVENT_NULL:    panic("Unhandled event ID = %d", e.event); break;
    case _EVENT_YIELD:   printf("This is event yield\n");           break;
    case _EVENT_SYSCALL: panic("Unhandled event ID = %d", e.event); break;
    default: panic("no event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
