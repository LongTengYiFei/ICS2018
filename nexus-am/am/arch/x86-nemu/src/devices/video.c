#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>
#define screen_port 0x100
static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;

size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;
      int data = inl(screen_port);
      info->width = data >> 16;
      info->height = data & 0xffff;
      return sizeof(_VideoInfoReg);
    }
  }
  return 0;
}

size_t video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _FBCtlReg *ctl = (_FBCtlReg *)buf;
      // Frame Buffer Controler 
      int i;
      for(i=0; i<= ctl->h-1; i++)//yi hang yi hang de xie shu ju
	      //write data row by row
	      //one pixel = 4 bytes
              memcpy(fb+(ctl->y+i)*screen_width()+ctl->x,ctl->pixels+i*ctl->w,ctl->w*4);
      if (ctl->sync) {
        // do nothing, hardware syncs.
      }
      return sizeof(_FBCtlReg);
    }
  }
  return 0;
}

void vga_init() {
}
