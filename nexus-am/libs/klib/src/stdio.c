#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

enum {Printf, Vsprintf};

void func_op(char *s1, const char s2, int func_num) {
  switch (func_num) {
    case Printf:   _putc(s2); break;
    case Vsprintf: *s1 = s2;  break;
    default: assert(0);
  }
}

int fmtop(char *out, const char *fmt, va_list ap, int func_num) {
  char *str;
  int len, d, ret = 0;
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
      case 'i':
      case 'd': {
        d = va_arg(ap, int);
        len = 0;
        while (d) {
          d_num[len++] = '0' + d%10;
          d /= 10;
        }
        for (int i = 0; i < len; ++i) {
          func_op(str, *(d_num+len-i-1), func_num);
          if (str != NULL) str++;
          ret++;
        }
        break;
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
  return 0;
}

#endif
