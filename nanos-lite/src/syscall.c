#include "common.h"
#include "syscall.h"

void sys_exit(_Context *c) {
  _halt(c->GPR2);
  // should not reach here
  assert(0);
}

void sys_yield(_Context *c) {
  _yield();
  c->GPRx = 0;
}

void sys_write(_Context *c) {
  if(c->GPR2 == 1 || c->GPR2 == 2) {
    char *buf = (char *)c->GPR3;
    for (int i = 0; i < c->GPR4; ++i)
      _putc(buf[i]);
    c->GPRx = c->GPR4;
  }
  else c->GPRx = -1;
  Log();
}

void sys_brk(_Context *c) {
  brk = (*c).GPR2;
  c->GPRx = 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit:           sys_exit(c);         break;
    case SYS_yield:          sys_yield(c);        break;
    //case SYS_open          sys_open(c);         break;
    //case SYS_read:         sys_read(c);         break;
    case SYS_write:          sys_write(c);        break;
    //case SYS_kill:         sys_kill(c);         break;
    //case SYS_getpid:       sys_getpid(c);       break;
    //case SYS_close:        sys_close(c);        break;
    //case SYS_lseek:        sys_lseek(c);        break;
    case SYS_brk:            sys_brk(c);          break;
    //case SYS_fstat:        sys_fstat(c);        break;
    //case SYS_time:         sys_time(c);         break;
    //case SYS_signal:       sys_signal(c);       break;
    //case SYS_execve:       sys_execve(c);       break;
    //case SYS_fork:         sys_fork(c);         break;
    //case SYS_link:         sys_link(c);         break;
    //case SYS_unlink:       sys_unlink(c);       break;
    //case SYS_wait:         sys_wait(c);         break;
    //case SYS_times:        sys_times(c);        break;
    //case SYS_gettimeofday: sys_gettimeofday(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
