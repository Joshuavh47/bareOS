#include <barelib.h>
#include <fs.h>

/* fs_write - Takes a file descriptor index into the 'oft', a  pointer to a  *
 *            buffer  that the  function reads data  from and the number of  *
 *            bytes to copy from the buffer to the file.                     *
 *                                                                           *
 *            'fs_write' reads data from the 'buff' and copies it into the   *
 *            file  'blocks' starting  at the 'head'.  The  function  will   *
 *            allocate new blocks from the block device as needed to write   *
 *            data to the file and assign them to the file's inode.          *
 *                                                                           *
 *  returns - 'fs_write' should return the number of bytes written to the    *
 *            file.                                                          */
uint32 fs_write(uint32 fd, char* buff, uint32 len) {
  
  if(oft[fd].state != FSTATE_OPEN){
    return -1;
  }
  
  int available_blocks = oft[fd].inode.size/fsd->device.blocksz + (oft[fd].inode.size/fsd->device.blocksz > 0);
  int blocks_needed = (oft[fd].head + len)/fsd->device.blocksz + ((oft[fd].head + len)%fsd->device.blocksz > 0);
  if(blocks_needed>INODE_BLOCKS){
    return -1;
  }
  int new_blocks_needed = (blocks_needed - available_blocks<0?0:blocks_needed - available_blocks);
  int count = 0;
  for(int i=0;i<fsd->device.nblocks;i++){
    if(count + available_blocks >= blocks_needed){
      break;
    }
    if(!fs_getmaskbit(i)){
      count++;
    }
  }
  if(count < new_blocks_needed){
    return -1;
  }
  count = 0;
  for(int i=0;i<fsd->device.nblocks;i++){
    if(count + available_blocks >= blocks_needed){
      break;
    }
    if(!fs_getmaskbit(i)){
      oft[fd].inode.blocks[available_blocks + count] = i;
      fs_setmaskbit(i);
      count++;
    }
  }
  int total_written = 0;
  int index = oft[fd].head/fsd->device.blocksz;
  if(index>=blocks_needed){
    index--;
  }
  while(total_written<len){
    int written_to_block = (fsd->device.blocksz-(oft[fd].head%fsd->device.blocksz)>len-total_written?len-total_written:fsd->device.blocksz-(oft[fd].head%fsd->device.blocksz));
    bs_write(oft[fd].inode.blocks[index], oft[fd].head%fsd->device.blocksz, buff+total_written, written_to_block);
    total_written += written_to_block;
    oft[fd].head += written_to_block;
    index++;
  }
  oft[fd].inode.size = (oft[fd].head>oft[fd].inode.size?oft[fd].head:oft[fd].inode.size);
  bs_write(oft[fd].inode.id, 0, &oft[fd].inode, sizeof(inode_t));
  return len;
  
}
