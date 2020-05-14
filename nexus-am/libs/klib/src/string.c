#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
//let's GO!
size_t strlen(const char *s) {
  int len = 0;
  for(int i=0;;i++)
     if(s[i] != '\0')
	     len ++;
     else break;
  return len;
}

char *strcpy(char* dst,const char* src) {
  assert((dst != NULL) && (src != NULL));
  for(int i=0;;i++){
     dst[i] = src[i];
     if(src[i] == '\0')
	break;
  }
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
        size_t size_src = strlen(src);
	if( n > size_src) return strncpy(dst, src, size_src);
	else // n <= size_src
	{
		char ch[n+1];
		char *head = ch;
		size_t i = 0;
		while( i < n ) {
			*(head+i) = *(src+i);
			i++;
		}
		ch[n] = '\0';
		head = ch;
		char *result = dst;
		while((*(dst++) = *(head++))) ;
		return result;
	}
}

char* strcat(char* dst, const char* src) {
  char *r = dst;
  dst += strlen(dst);
  for(int i=0;;i++){
    *dst = src[i];
    dst ++;
    if(src[i] == '\0')
      break;
  }
  return r;
}

int strcmp(const char* s1, const char* s2) {
  if(strlen(s1) > strlen(s2))return 1;
  if(strlen(s1) < strlen(s2))return -1;
  for(int i=0;i<=strlen(s1) - 1;i++)
	  if(s1[i] > s2[i])
		  return 1;
          else if(s1[i] < s2[i])
		  return -1; 
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  if(strlen(s1) < strlen(s2))return -1;
  if(strlen(s1) > strlen(s2))return 1;
  for(int i=0;i<=n-1;i++)
	  if(s1[i]>s2[i])return 1;
          else if(s1[i]<s2[i])return -1;
  return 0;
}

void* memset(void* v,int c,size_t n) {
  void * ret = v;
  while(n--){
     *(char *)v = (char)c;
     v = (char*)v + 1;
  }
  return ret;
}

void* memcpy(void* out, const void* in, size_t n) {
  unsigned char *src = (unsigned char *) in;
  unsigned char *dst = (unsigned char *) out;

  for (int i = 0 ; i < n ; ++i) {
    *(dst + i) = *(src + i);
  }

  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){ 
const unsigned char *p1 = s1, *p2 = s2;
  while(n--)
    if( *p1 != *p2 )
      return *p1 - *p2;
    else
      p1++,p2++;
  return 0;
}

#endif
