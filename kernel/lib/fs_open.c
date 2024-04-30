#include <barelib.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*  Search for a filename  in the directory, if the file doesn't exist  *
 *  or it is  already  open, return  an error.   Otherwise find a free  *
 *  slot in the open file table and initialize it to the corresponding  *
 *  inode in the root directory.                                        *
 *  'head' is initialized to the start of the file.                     */
int32 fs_open(char* filename) {
  if(fsd->root_dir.numentries <= 0){
    return -1;
  }
  int index = -1;
  for(int i=0;i<fsd->root_dir.numentries;i++){
    if(strcmp((fsd->root_dir.entry[i].name), filename) == 0){
      index = i;
      break;
    }
  }
  if(index == -1){
    return -1;
  }

  for(int i=0;i<NUM_FD;i++){
    if(!strcmp(fsd->root_dir.entry[oft[i].direntry].name,filename)){
      if(oft[i].state == FSTATE_OPEN){
        return -1;
      }
    }
  }
  for(int i=0;i<NUM_FD;i++){
    if(oft[i].state == FSTATE_CLOSED){
      oft[i].state = FSTATE_OPEN;
      oft[i].head = 0;
      oft[i].direntry = index;
      bs_read(fsd->root_dir.entry[index].inode_block, 0, &(oft[i].inode), sizeof(inode_t));
      return i;
    }
    
  }
  return -1;

}
