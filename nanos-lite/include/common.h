#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_CTE
//#define HAS_VME

#include <am.h>
#include <klib.h>
#include "debug.h"

typedef char bool;
void naive_uload();
size_t get_ramdisk_size();
size_t ramdisk_read();
size_t ramdisk_write();
size_t serial_write(const void *buf, size_t offset, size_t len) ;
size_t events_read();
size_t dispinfo_read();
size_t fb_write();
#define true 1
#define false 0

#endif
