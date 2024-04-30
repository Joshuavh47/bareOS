#include <barelib.h>
#include <fs.h>

/* fs_read - Takes a file descriptor index into the 'oft', a  pointer to a  *
 *           buffer that the function writes data to and a number of bytes  *
 *           to read.                                                       *
 *                                                                          *
 *           'fs_read' reads data starting at the open file's 'head' until  *
 *           it has  copied either 'len' bytes  from the file's  blocks or  *
 *           the 'head' reaches the end of the file.                        *
 *                                                                          *
 * returns - 'fs_read' should return the number of bytes read (either 'len' *
 *           or the  number of bytes  remaining in the file,  whichever is  *
 *           smaller).                                                      */
uint32 fs_read(uint32 fd, char* buff, uint32 len) {
  if(oft[fd].state == FSTATE_CLOSED){
    return -1;
  }
  int amount = ((oft[fd].inode.size-oft[fd].head)<len?(oft[fd].inode.size-oft[fd].head):len);
  int read = 0;
  int headIndex = oft[fd].head/fsd->device.blocksz;
  while(read<amount){
    int sizeOfRead = fsd->device.blocksz-(oft[fd].head%fsd->device.blocksz)>amount-read?amount-read:fsd->device.blocksz-(oft[fd].head%fsd->device.blocksz);
    bs_read(oft[fd].inode.blocks[headIndex], oft[fd].head%fsd->device.blocksz, buff+read, ((fsd->device.blocksz-(oft[fd].head%fsd->device.blocksz))>(amount-read)?amount-read:fsd->device.blocksz-(oft[fd].head%fsd->device.blocksz)));
    read+=sizeOfRead;
    oft[fd].head+=sizeOfRead;
    headIndex++;
  }
  return amount;
}
