#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

enum { Printf, Vsprintf };

void func_op(char *s1, const char s2, int func_num) {
  switch (func_num) {
    case Printf:   _putc(s2); break;
    case Vsprintf: *s1 = s2;  break;
    default: assert(0);
  }
}

int fmtop(char *out, const char *fmt, va_list ap, int func_num) {
  char *str;
  int len, d, ret = 0, base;
  uintptr_t p;
  char* s;
  char c;
  char d_num[30];
  for (str = out; *fmt; ++fmt) {
    if (*fmt != '%') {
      func_op(str, *fmt, func_num);
      if (str != NULL) str++;
      ret++;
      continue;
    }
    fmt++;
    switch (*fmt) {
      case 's': {
        s = va_arg(ap, char*);
        len = strlen(s);
        for (int i = 0; i < len; ++i) {
          func_op(str, *(s+i), func_num);
          if (str != NULL) str++;
          ret++;
        }
        break;
      }
      case 'c': {
        c = (char) va_arg(ap, int);
        func_op(str, c, func_num);
        if (str != NULL) str++;
        ret++;
        break;
      }
      case 'x': 
      case 'i':
      case 'd': {
        if (*fmt == 'x') base = 16;
        else base = 10;
        d = va_arg(ap, int);
        len = 0;
        if (d == 0) d_num[len++] = '0';
        else while (d) {
          d_num[len++] = (d%base < 10) ? ('0' + d%base) : ('a' + d%base - 10);
          d /= base;
        }
        /*for (int i = 0; i < len; ++i) {
          func_op(str, d_num[len-i-1], func_num);
          if (str != NULL) str++;
          ret++;
        }*/
        break;
      }
      case 'p': {
        p = (uintptr_t)va_arg(ap, void *);
        len = 0;
        if (p == 0) d_num[len++] = '0';
        else while (p) {
          d_num[len++] = (p%16 < 10) ? ('0' + p%16) : ('a' + p%16 - 10);
          p /= 16;
        }
        func_op(str, '0', func_num);
        func_op(str, 'x', func_num);
        for (int i = 0; i < len; ++i) {
          func_op(str, *(d_num+len-i-1), func_num);
          if (str != NULL) str++;
          ret++;
        }
        break;
      }
      default: {
        printf("\n\n'%c' is not implemented\n\n", *fmt);
        assert(0);
      }
    }
  }
  if (str != NULL) *str = '\0';
  return ret;
}

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  va_end(ap);
  return fmtop(NULL, fmt, ap, Printf);
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return fmtop(out, fmt, ap, Vsprintf);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  int ret;
  va_start(ap, fmt);
  ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  assert(0);
  return 0;
}

#endif
