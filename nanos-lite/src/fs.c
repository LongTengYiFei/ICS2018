#include "fs.h"
#include "string.h"
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char *pathname, int flags, int mode){
    //we can ignore the flags and mode
    for(int i=0;i<=NR_FILES;i++)
       if(strcmp(pathname, file_table[i].name) == 0)
       {       
	       file_table[i].open_offset = 0;
	       return i; 
       }
    
    //no found
    assert(0);
    return -1;
}

int fs_close(int fd){
//we need set the open offset to 0
//becase this is close
  file_table[fd].open_offset = 0; 
//we presume we always close success, so we return 0
  return 0;
}

size_t fs_filesz(int fd){
	return file_table[fd].size;
}

size_t fs_read(int fd, void*buf, size_t len){
  size_t fs_size = fs_filesz(fd);
  size_t disk_off = file_table[fd].disk_offset;
  size_t open_off = file_table[fd].open_offset; 
  if(open_off >= fs_size)
	  return 0;
  if(open_off + len > fs_size)
	  len = fs_size - open_off;

  ramdisk_read(buf, disk_off + open_off, len);  
  file_table[fd].open_offset += len;
  /*
  printf("size = %d\n",fs_size);
  printf("off = %d\n",disk_off);
  printf("op = %d\n",open_off);*/
  return len; 
}

size_t fs_write(int fd, const void*buf, size_t len){


return 0;
}

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
