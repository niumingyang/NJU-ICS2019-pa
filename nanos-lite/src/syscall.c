#include "common.h"
#include "syscall.h"
#include "proc.h"
#include <unistd.h>

void naive_uload(PCB *pcb, const char *filename);

uintptr_t sys_execve(const char *fname) {
  naive_uload(NULL, fname);
  return 0;
}

void sys_exit(int status) {
  //sys_execve("/bin/init");
  _halt(status);

  // should not reach here
  assert(0);
}

intptr_t sys_yield() {
  _yield();
  return 0;
}

int fs_open(const char *path, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t count);
ssize_t fs_write(int fd, const void *buf, size_t count);
int fs_close(int fd);
off_t fs_lseek(int fd, off_t offset, int whence);

intptr_t sys_brk(intptr_t _brk_) {
  return 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit:           sys_exit(a[1]);                                  break;
    case SYS_yield:          c->GPRx = sys_yield();                           break;
    case SYS_open:           c->GPRx = fs_open((const char *)a[1],a[2],a[3]); break;
    case SYS_read:           c->GPRx = fs_read(a[1],(void *)a[2],a[3]);       break;
    case SYS_write:          c->GPRx = fs_write(a[1],(void *)a[2],a[3]);      break;
    //case SYS_kill:         c->GPRx = sys_kill();                            break;
    //case SYS_getpid:       c->GPRx = sys_getpid();                          break;
    case SYS_close:          c->GPRx = fs_close(a[1]);                        break;
    case SYS_lseek:          c->GPRx = fs_lseek(a[1],a[2],a[3]);              break;
    case SYS_brk:            c->GPRx = sys_brk(a[1]);                         break;
    //case SYS_fstat:        c->GPRx = sys_fstat();                           break;
    //case SYS_time:         c->GPRx = sys_time();                            break;
    //case SYS_signal:       c->GPRx = sys_signal();                          break;
    case SYS_execve:         c->GPRx = sys_execve((char *)a[0]);              break;
    //case SYS_fork:         c->GPRx = sys_fork();                            break;
    //case SYS_link:         c->GPRx = sys_link();                            break;
    //case SYS_unlink:       c->GPRx = sys_unlink();                          break;
    //case SYS_wait:         c->GPRx = sys_wait();                            break;
    //case SYS_times:        c->GPRx = sys_times();                           break;
    //case SYS_gettimeofday: c->GPRx = sys_gettimeofday();                    break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
