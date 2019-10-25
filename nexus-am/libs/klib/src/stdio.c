#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *str;
  int len, d;
  char* s;
  char d_num[30];
  for (str = out; *fmt; ++fmt) {
    if (*fmt != '%') {
      *str++ = *fmt;
      continue;
    }
    fmt++;
    switch (*fmt) {
      case 'c': {
        *str++ = (char) va_arg(ap, int);
        break;
      }
      case 's': {
        s = va_arg(ap, char*);
        len = strlen(s);
        for (int i = 0; i < len; ++i) *str++ = *s++;
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
        for (int i = 0; i < len; ++i) *str++ = d_num[len-i-1];
        break;
      }
    }
  }
  *str = '\0';
  return str - out;
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
