#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int ret = 0;
  //while (*s++) ++ret;
  return ret;
}

char *strcpy(char* dst,const char* src) {
  char* tmp = dst;
  //while (*src != '\0')
  //  *dst++ = *src++;
  *dst = '\0';
  return tmp;
}

char* strncpy(char* dst, const char* src, size_t n) {
  char* tmp = dst;
  //while (*src != '\0'&&n-- > 0)
  //  *dst++ = *src++;
  *dst = '\0';
  return tmp;
}

char* strcat(char* dst, const char* src) {
  char* tmp = dst;
  //while(*dst) dst++;
  //while (*src != '\0')
  //  *dst++ = *src++;
  *dst = '\0';
  return tmp;
}

int strcmp(const char* s1, const char* s2) {
  int ret = 0;
  //while (!(ret = *(unsigned char*)s1 - *(unsigned char*)s2)&&*s2)
  //  ++s1, ++s2;
  if (ret < 0) return -1;
  else if (ret > 0) return 1;
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  int ret = 0;
  //while (!(ret = *(unsigned char*)s1 - *(unsigned char*)s2)&&*s2&&n>0)
  //  ++s1, ++s2, --n;
  if (ret < 0) return -1;
  else if (ret > 0) return 1;
  return 0;
}

void* memset(void* v,int c,size_t n) {
  //unsigned char uc = c;
  //unsigned char *s;
  //for (s = v; n > 0; ++s, --n)
  //  *s = uc;
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  unsigned char* dst = (unsigned char*)out;
  //unsigned char* src = (unsigned char*)in;
  unsigned char* tmp = dst;
  //while (n-- > 0)
  //  *dst++ = *src++;
  return tmp;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if (!n)
    return 0;
  unsigned char* v1 = (unsigned char*)s1;
  unsigned char* v2 = (unsigned char*)s2;
  //while (*v1 == *v2 && n > 0)
  //  v1++, v2++, --n;
  if (n == 0) return 0; 
  return *v1 - *v2;
}

#endif
