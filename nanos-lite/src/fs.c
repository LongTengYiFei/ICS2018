#include "fs.h"
#include "string.h"
#include "stdlib.h"
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

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_TTY};

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
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  [FD_FB] = {"/dev/fb", 0, 0, 0, NULL, fb_write},
  [FD_EVENTS] = {"/dev/events", 0, 0, 0, events_read,invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 128 ,0, 0, dispinfo_read, invalid_write},
  [FD_TTY] = {"/dev/tty", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char *pathname, int flags, int mode){
    //we can ignore the flags and mode
    for(int i=0;i<=NR_FILES-1;i++)
       if(strcmp(file_table[i].name, pathname) == 0)
       {       
	        file_table[i].open_offset = 0;
		Log("fd = %d",i);
	        return i; 
       } 
    //no found
    assert(0);
    return -1;
}

int fs_close(int fd){
	Log("fd = %d",fd);
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
	Log("fd = %d",fd);
	switch(fd){
	  case FD_STDIN:
	  case FD_STDOUT:
	  case FD_STDERR:
	  case FD_FB:
		  break;
	  case FD_EVENTS:
             len =  file_table[fd].read(buf, 0, len);
	     break;
	  case FD_DISPINFO:
	     if(file_table[fd].open_offset >= fs_size)
		 return 0;
	     if(file_table[fd].open_offset + len > fs_size)
		 len = fs_size - file_table[fd].open_offset;
             file_table[fd].read(buf, file_table[fd].open_offset, len);
	     file_table[fd].open_offset += len;
	     break;
	  default:
            if(file_table[fd].open_offset >= fs_size)
	        return 0;
            if(file_table[fd].open_offset + len > fs_size)
	        len = fs_size - file_table[fd].open_offset;

            ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);  
            file_table[fd].open_offset += len;
	    break;
  }
  return len; 
}

size_t fs_write(int fd, void*buf, size_t len){
	Log("fd = %d",fd);
  size_t fs_size = fs_filesz(fd);
  switch(fd){
    case FD_STDIN:
	    break;
    case FD_TTY:
    case FD_STDOUT:
    case FD_STDERR:
	 file_table[fd].write(buf, 0, len);
	 break;
    case FD_EVENTS:
    case FD_DISPINFO:
	 break;
    case FD_FB:
	 if(file_table[fd].open_offset >= fs_size)
		 return 0;
	 if(file_table[fd].open_offset + len > fs_size)
		 len = fs_size - file_table[fd].open_offset;
         file_table[fd].write(buf, file_table[fd].open_offset, len);
	 file_table[fd].open_offset += len;
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
	Log("fd = %d",fd);
   size_t result = -1;
   switch(whence){
         case SEEK_SET:
		 if(offset >= 0 && offset <=fs_filesz(fd)){
		    file_table[fd].open_offset = offset;
		    result = offset;
                 }
		 break;
	 case SEEK_CUR:
		 if((offset + file_table[fd].open_offset >= 0) && (offset + file_table[fd].open_offset <= fs_filesz(fd)))
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
  // RGBA
  file_table[FD_FB].size = 4 * screen_width() * screen_height();
}
