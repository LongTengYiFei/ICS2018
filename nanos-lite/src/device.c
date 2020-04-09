#include "common.h"
#include <amdev.h>
#include <stdio.h>
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
   int key = read_key();
   bool down = false;
  
   if(key & 0x8000){
	   key ^=0x8000;
	   down = true;
   }
   if(key == _KEY_NONE){
	   uint32_t t = uptime();
	   //Log("strlen(buf) = %d",strlen(buf));
	   //Log("t = %d",t);
	   /*
	   strncpy(buf, "t ",2);
           char tmp[10];
	   int i=0;
	   while(t != 0)
	   {
	      tmp[i] = t%10 + '0';
	      i++;
	      t /= 10; 
	   }
	   i--;
	   int j=0;
	   for(;i>=0;i--){
	      strncpy(buf +2 +j, tmp+i, 1);
	      j++;
	   }
	   strncpy(buf +2 +j, "\n\0", 2);
	   */
	   //Log("buf's value = 0x%x",buf);
	   //Log("strlen(buf) = %d",strlen(buf));
	   //Log("sizeof(buf) = %d",sizeof(buf));
	   sprintf(buf, "t %d\n",t);
   }else{
	   /*
	   if(down == true){
	       strncpy(buf, "kd ", 3);
	       strncpy(buf+3, keyname[key], strlen(keyname[key]));
	       strncpy(buf+3+strlen(keyname[key]), "\n\0", 2);
	   }else{
	       strncpy(buf, "ku ", 3);
	       strncpy(buf+3, keyname[key], strlen(keyname[key])); 
	       strncpy(buf+3+strlen(keyname[key]), "\n\0", 2);
	   }
	   */
	   sprintf(buf, "%s %s\n",down ? "kd": "ku", keyname[key]);
   }
   //Log("buf = %s",buf);
   return strlen(buf);
}

static char dispinfo[128] __attribute__((used));
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	//Log("strlen(buf) = %d\n",strlen(buf));
        strncpy(buf, dispinfo + offset, len);
	//Log("dispinfo_read:%s offset:%d len:%d",buf, offset, len);
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
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", screen_width(), screen_height());
}
