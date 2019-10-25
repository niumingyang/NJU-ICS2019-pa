#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int d, len, ret = 0;
  char c, *s;
  while (*fmt) 
    switch (*fmt++) {
      case 's':{
        s = va_arg(ap, char *);
        len = strlen(s);
        for (int i = 0; i < len; ++i) *out++ = *s++;
        ret += len;
        break;
      }
      case 'i':
      case 'd':{
        d = va_arg(ap, int);
        char tmp[100];
        int cnt = 0;
        while (d) {
          tmp[cnt++] = '0' + d%10;
          d /= 10;
        }
        ret += cnt;
        while (cnt--)
          *out++ = tmp[cnt];
        break;
      }
      case 'c':{
        c = (char)va_arg(ap, int);
        *out++ = c;
        ret++;
        break;
      }
      default: assert(0);
    }
  return ret;
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
