#include <am.h>
#include <x86.h>
#include <amdev.h>
#include<stdio.h>
unsigned long long now;
size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _UptimeReg *uptime = (_UptimeReg *)buf;
      now = inl(0x48);
      printf("come in to timer_read\n");
      printf("now is %lld\n",now);
      uptime->hi = now >> 32;
      uptime->lo = now & 0x00000000ffffffff;
      return sizeof(_UptimeReg);
    }
    case _DEVREG_TIMER_DATE: {
      _RTCReg *rtc = (_RTCReg *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2020;
      return sizeof(_RTCReg);
    }
  }
  return 0;
}

void timer_init() {
}
