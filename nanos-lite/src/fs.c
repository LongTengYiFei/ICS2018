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
  {"stdin", 0, 0, 0, invalid_read, serial_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, },
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char *pathname, int flags, int mode){
    printf("come into fs_open\n");
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
  return len; 
}

size_t fs_write(int fd, void*buf, size_t len){
  size_t fs_size = fs_filesz(fd);
  char *tmp = buf;
  switch(fd){
    case FD_STDOUT:
         for(int i=0;i<=len-1;i++)
             _putc(*(tmp++));
         break;

    default:     
           if(file_table[fd].open_offset >= fs_size)
	      return 0;
           if(file_table[fd].open_offset + len > fs_size)
	      len = fs_size - file_table[fd].open_offset;
           ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);  
           file_table[fd].open_offset += len;
	   break;
  }
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence){
   size_t result = -1;
   switch(whence){
         case SEEK_SET:
		 if(offset>0 && offset <=fs_filesz(fd)){
		    file_table[fd].open_offset = offset;
		    result = offset;
                 }
		 break;
	 case SEEK_CUR:
		 if((offset + file_table[fd].open_offset >= 0) && (offset + file_table[fd].open_offset <=fs_filesz(fd)))
		 {
			 file_table[fd].open_offset += offset;
		         result = file_table[fd].open_offset;
		 }
	        break;
	 case SEEK_END:
		 file_table[fd].open_offset = offset + fs_filesz(fd);
		 result = file_table[fd].open_offset;
	        break;	
   }
   return result;
}
void init_fs() {
  // TODO: initialize the size of /dev/fb
}
