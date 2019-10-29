#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>

static inline int min(int x, int y) {
  return (x < y) ? x : y;
}

extern uint32_t *fb;

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      uint32_t wh = inl(0x100);
      info->width = (wh & 0xffff0000) >> 16;
      info->height = wh & 0xffff;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      int x, y, w, h;
      uint32_t* pixels;
      ctl->x = x = inl(0x104);
      ctl->y = y = inl(0x108);
      ctl->pixels = pixels = (void *)inl(0x10c);
      ctl->w = w = inl(0x110);
      ctl->h = h = inl(0x114);
      _DEV_VIDEO_INFO_t *_info = {0};
      __am_video_read(_DEVREG_VIDEO_INFO, _info, 0);
      int W = _info->width;
      int H = _info->height;
      int cp_bytes = sizeof(uint32_t) * min(w, W - x);
      for (int j = 0; j < h && y + j < H; j ++) {
        memcpy(&fb[(y + j) * W + x], pixels, cp_bytes);
        pixels += w;
      }
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
