#include <barelib.h>
#include <fs.h>

extern fsystem_t* fsd;

/*  Search for 'filename' in the root directory.  If the  *
 *  file exists,  returns an error.  Otherwise, create a  *
 *  new file  entry in the  root directory, a llocate an  *
 *  unused  block in the block  device and  assign it to  *
 *  the new file.                                         */
int32 fs_create(char* filename) {
  if(strlen(filename)>FILENAME_LEN){
    return -1;
  }
  if(fsd->root_dir.numentries == DIR_SIZE){
    return -1;
  }
  int found = 0;
  for(int i=0;i<fsd->root_dir.numentries;i++){
    if(strcmp((fsd->root_dir.entry[i].name), filename) == 0){
      found = 1;
    }
  }
  if(found == 1){
    return -1;
  }
  int block = -1;
  for(int i=0;i<INODE_BLOCKS;i++){
    if(fs_getmaskbit(i) == 0){
      block = i;
      break;
    }
  }
  if(block == -1){
    return -1;
  }
  dirent_t newDirEnt;
  newDirEnt.inode_block = block;
  for(int i=0;i<FILENAME_LEN;i++){
    newDirEnt.name[i] = filename[i];
  }
  

  inode_t newInode;
  newInode.id = block;
  newInode.size = 0;
  //newInode.blocks[0] = block;

  fsd->root_dir.entry[fsd->root_dir.numentries] = newDirEnt;
  fsd->root_dir.numentries++;

  bs_write(block, 0, &newInode, sizeof(inode_t));
  
  fs_setmaskbit(block);
  
  bs_write(BM_BIT, 0, fsd->freemask, fsd->freemasksz);
  
  return 0;

}
