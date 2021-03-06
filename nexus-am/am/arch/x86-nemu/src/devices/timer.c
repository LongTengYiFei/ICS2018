#include <am.h>
#include <x86.h>
#include <amdev.h>
static uint32_t start_time;
size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _UptimeReg *uptime = (_UptimeReg *)buf;
      uint32_t now = inl(0x48);
      uptime->hi = 0;
      uptime->lo = now - start_time;
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
	start_time = inl(0x48);
}
