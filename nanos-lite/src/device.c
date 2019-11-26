#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (int i = 0; i < len; ++i)
    _putc(((const char *)buf)[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  int key = read_key();
  bool keydown = false;
  if (key & 0x8000) {
    key ^= 0x8000;
    keydown = true;
  }
  if (key == _KEY_NONE) {
    uint32_t time = uptime();
    sprintf(buf, "t %d\n", time);
  }
  else sprintf(buf, "%s %s\n", keydown?"kd":"ku", keyname[key]);
  return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  memcpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int W = screen_width(), H = screen_height();assert(0);
  int x = offset % W, y = offset / W + 1;
  uint32_t *pixels = (uint32_t *)buf;
  draw_rect(pixels, x, y, W, H);
  return 0;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
  sprintf(dispinfo, "WIDTH:%d\nHEIGNT:%d", screen_width(), screen_height);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
