#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for(int i =0;i<=len-1;i++)
	  _putc(((char *)buf)[i]);
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
        strncpy(buf, dispinfo + offset, len);
	Log("dispinfo_read:%s offset:%d len:%d",buf, offset, len);
	return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int row, col;
  offset /= 4;
  row = offset / screen_width();
  col = offset % screen_width();

  draw_rect((uint32_t *)buf, col, row, len/4, 1);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
