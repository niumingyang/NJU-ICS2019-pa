#include "common.h"
#include "syscall.h"

void sys_yield(_Context *c) {
  _yield();
  c->GPRx = 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    //case SYS_exit:
    case SYS_yield:          sys_yield(c); break;
    //case SYS_open:
    //case SYS_read:
    //case SYS_write:
    //case SYS_kill:
    //case SYS_getpid:
    //case SYS_close:
    //case SYS_lseek:
    //case SYS_brk:
    //case SYS_fstat:
    //case SYS_time:
    //case SYS_signal:
    //case SYS_execve:
    //case SYS_fork:
    //case SYS_link:
    //case SYS_unlink:
    //case SYS_wait:
    //case SYS_times:
    //case SYS_gettimeofday:
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
