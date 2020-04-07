#include <stdio.h>

int main() {
  FILE *fp = fopen("/dev/events", "r");
  ///printf("sizeof(fp) = %d\n",sizeof(fp));
  volatile int j = 0;
  while(1) {
    j ++;
    if (j == 1000000) {
      char buf[256];
      char *p = buf, ch;
      //printf("pending fgetc\n");
      while ((ch = fgetc(fp)) != -1) {
        //printf("fgetc\n");
        *p ++ = ch;
        if(ch == '\n') {
          *p = '\0';
          break;
        }
      }
      printf("receive event: %s", buf);
      j = 0;
    }
  }

  fclose(fp);
  return 0;
}

