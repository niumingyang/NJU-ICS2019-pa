#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>

uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
static int W = 400;
static int H = 300;

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
      int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
      uint32_t *pixels = ctl->pixels;
      for (int j = 0; j < h && y + j < H; ++j) {
        int tmp = (y + j) * W + x;
        for (int i = 0; i < w && x + i < W; ++i)
          fb[tmp + i] = pixels[i];
        pixels += w;
      }
      if (ctl->sync) {
        outl(SYNC_ADDR, ctl->sync);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  for (i = 0; i < size; i ++) fb[i] = 0;
  draw_sync();
}
