#include <barelib.h>
#include <fs.h>

/* fs_write - Takes a file descriptor index  into the 'oft' and an  integer  *
 *            offset.  'fs_seek' moves the current head to match the given   *
 *            offset, bounded by the size of the file.                       *
 *                                                                           *
 *  returns - 'fs_seek' should return the new position of the file head      */
uint32 fs_seek(uint32 fd, uint32 offset, uint32 relative) {
  if(fd<0){
    return -1;
  }
  if(relative == SEEK_START){
    if(offset<0){
      return -1;
    }
    oft[fd].head = offset;
  }
  else if(relative == SEEK_END){
    if(offset>0){
      return -1;
    }
    oft[fd].head = oft[fd].inode.size + offset;
  }
  else if(relative == SEEK_HEAD){
    if(oft[fd].head + offset > oft[fd].inode.size || oft[fd].head + offset < 0){
      return -1;
    }
    oft[fd].head += offset;
  }
  return oft[fd].head;
  
}
