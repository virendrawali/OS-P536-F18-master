#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;
int inode_id = 2;

#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

//  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

/**************************************************************************************
 Function to open file, if file present in root directory then extract information from 
 root directory and make a entry of file table into open file table and return index of 
 table as a file descriptor to calling function. Otherwise create new entry for file 
 same as fs_create()
**************************************************************************************/

int fs_open(char *filename, int flags) {
	int inode_number = 0;
	int i = 0;
	int j = 0;
	struct inode file_inode;
	struct filetable file_info;
	struct directory d = fsd.root_dir;
	printf("we are in open file:%d\n",fsd.root_dir.numentries);
	for (i = 0; i < d.numentries; i++)
	{
		if (strncmp(d.entry[i].name, filename,strlen(filename)) == 0)
		{
			inode_number = d.entry[i].inode_num;
			fs_get_inode_by_num(0, inode_number, &file_inode);
			if(next_open_fd>=NUM_FD)
			{
				printf("%d files are opened\n",NUM_FD);
				return SYSERR;
			}
			file_info.state = FSTATE_OPEN;
			file_info.fileptr = 0;
			file_info.de = &(d.entry[i]);
			file_info.in = file_inode;
			for(j = 0; j<NUM_FD; j++)
               		{
                        	if(oft[j].state == 0)
                        	{
                                	next_open_fd = j;
                                	break;
                        	}
                	}
			if(j>=NUM_FD)
			{
				printf("No space in oft\n");
				return SYSERR;
			}	
			oft[next_open_fd] = file_info;
			return next_open_fd;
		}
	}		
	if(flags == 0 || flags == 1 || flags == 2)
	{
		struct inode file_inode;
                struct filetable file_info;
                int i = 0;
                file_inode.id = inode_id;
                inode_id = inode_id + 1;
                fs_put_inode_by_num(0,file_inode.id,&file_inode);
                int inode_bit = file_inode.id / INODES_PER_BLOCK;
                inode_bit = inode_bit + FIRST_INODE_BLOCK;
                fs_setmaskbit(inode_bit);
                file_inode.type=INODE_TYPE_FILE;
                file_inode.size = sizeof(struct inode);
                for (i = 0;i<INODEBLOCKS;i++)
                {
                        file_inode.blocks[i] = 0;
                }
                file_info.state = FSTATE_OPEN;
                file_info.fileptr = 0;
                if(strlen(filename) >= FILENAMELEN)
                {
                        printf("File name is greater than 32 characters\n");
                        return 0;

                }
                strncpy(file_info.de->name,filename,strlen(filename));
                file_info.in = file_inode;
                file_info.de->inode_num = file_inode.id;
		fsd.root_dir.entry[fsd.root_dir.numentries].inode_num = file_inode.id;
                strncpy(fsd.root_dir.entry[fsd.root_dir.numentries].name,filename,strlen(filename));
                fsd.root_dir.numentries = fsd.root_dir.numentries + 1;
                fsd.inodes_used = fsd.inodes_used + 1;
                for(j = 0; j<NUM_FD; j++)
                {
                        if(oft[j].state == 0)
                        {
                                next_open_fd = j;
                                break;
                        }
                }
		if(j>=NUM_FD)
                {
                    printf("No space in oft\n");
                    return SYSERR;
                }
		oft[next_open_fd] = file_info;
                return next_open_fd;
	}
	else
	{	
		return SYSERR;
	}
}

/**************************************************************************************
 Function is to close the given file and chnage the state of file to FSTATE_CLOSE to
 denode file is closed and the netry of this file descriptor in open file table will
 be used by other files.
**************************************************************************************/

int fs_close(int fd) {
	if(fd>=0)
	{
		oft[fd].state = FSTATE_CLOSED;
		return 1;	
	}
	return SYSERR;
}

/**************************************************************************************
 Function is to create new file, while creating new file we need to assign new inode
 for each new file and have to create new file table entry. Set all the attributes for
 inode and file table and also update root directory of file system.
**************************************************************************************/

int fs_create(char *filename, int mode) {
  if(mode==O_CREAT)
        {
		struct inode file_inode;
		struct filetable file_info;
		int i = 0;
		int j = 0;
		file_inode.id = inode_id;
		inode_id = inode_id + 1;
                fs_put_inode_by_num(0,file_inode.id,&file_inode);
                int inode_bit = file_inode.id / INODES_PER_BLOCK;
                inode_bit = inode_bit + FIRST_INODE_BLOCK;
                fs_setmaskbit(inode_bit);
		file_inode.type=INODE_TYPE_FILE;
                file_inode.size = sizeof(struct inode);
		for (i = 0;i<INODEBLOCKS;i++)
       		{
                	file_inode.blocks[i] = 0;
        	}
		file_info.state = FSTATE_OPEN;
       		file_info.fileptr = 0;
		file_info.de->inode_num = file_inode.id;
		if(strlen(filename) >= FILENAMELEN)
		{
			printf("File name is greater than 32 characters\n");
			return 0;
			
		}
		strncpy(file_info.de->name,filename,strlen(filename));
		file_info.in = file_inode;
		fsd.root_dir.entry[fsd.root_dir.numentries].inode_num = file_inode.id;
		strncpy(fsd.root_dir.entry[fsd.root_dir.numentries].name,filename,strlen(filename));
                fsd.root_dir.numentries = fsd.root_dir.numentries + 1;
		fsd.inodes_used = fsd.inodes_used + 1;
		for(j = 0; j<NUM_FD; j++)
		{
			if(oft[j].state == 0)
			{
				next_open_fd = j;
				break;
			}
		}
		if(j>=NUM_FD)
                {
                    printf("No space in oft\n");
                    return SYSERR;
                }
		oft[next_open_fd] = file_info;
		return next_open_fd;
		 
        }
        else
        {
		printf("Mode is other than O_CREATE\n");
                return SYSERR;
        }
	
}

/**************************************************************************************
 Function to move the pointer to specified location from current location. User will 
 specify the pointer offset which we need to move back frem current file pointer 
 position.
**************************************************************************************/

int fs_seek(int fd, int offset) {
	struct filetable file_info = oft[fd];
	if(fd >=0)
	{
	   if(file_info.fileptr + offset<0)
	   {
		printf("Invalid file pointer\n");
		return SYSERR;
	   }
           file_info.fileptr = file_info.fileptr + offset;
	   oft[fd] = file_info;
	   return file_info.fileptr;
		
	}
	else
	{
		return SYSERR;
	}
}

/**************************************************************************************
 Function is to read the data from data blocks where the data for given file descriptor 
 is stored.
**************************************************************************************/

int fs_read(int fd, void *buf, int nbytes)
{
    int num_bytes = nbytes;
    int block_index = 0;
    struct filetable file_info=oft[fd];
    struct inode in = file_info.in;
    int inode_block= (file_info.fileptr / fsd.blocksz);
    int inode_offset=(file_info.fileptr % fsd.blocksz);
    if(fd >= 0)
    {
         if(file_info.state == 1)
         {
			block_index = file_info.in.blocks[inode_block];
			while(num_bytes>0)
			 {
            			if(num_bytes<(fsd.blocksz-inode_offset))
				{
                			bs_bread(0,block_index,inode_offset,buf,num_bytes);
                			file_info.fileptr=file_info.fileptr + num_bytes;
                			buf= buf + num_bytes;
                			num_bytes=0;
                			return nbytes;
            			}
            			else
				{
                			if(inode_block==INODEBLOCKS-1)
					{
                    				return nbytes - num_bytes;
                			}
                			bs_bread(0,block_index,inode_offset,buf,fsd.blocksz-inode_offset);
					buf = buf + (fsd.blocksz-inode_offset);
                			file_info.fileptr = file_info.fileptr + (fsd.blocksz-inode_offset);
					num_bytes= num_bytes - (fsd.blocksz-inode_offset);
                			oft[fd] = file_info;
 					inode_block = inode_block + 1;
					block_index=in.blocks[inode_block];
                			inode_offset=0;
            			}
        		   }
	   return nbytes;
	   }
	   else
	   {
		printf("File is not open\n");
		return SYSERR;
	   }
     }
     else
     {
	 return SYSERR;
     }
}

/**************************************************************************************
 Function is to write the data into empty data blocks and store data block information
 in inode data structure which will be used by fs_read to read the data.
**************************************************************************************/

int fs_write(int fd, void *buf, int nbytes) {
struct filetable file_info=oft[fd];
int num_bytes = nbytes;
int block_index = 0;
int i = 0;
struct inode in = file_info.in;
int inode_block= file_info.fileptr / fsd.blocksz;
int inode_offset= file_info.fileptr % fsd.blocksz;
if(fd >= 0)
{
         if(file_info.state == 1)
         {
     		     while(num_bytes>0)
		     {
     			if(in.blocks[inode_block]==0)
			{
				
				 for(i=0;i<fsd.nblocks;i++)
				 {
        				if(fs_getmaskbit(i)==0)
					{
            					block_index = i;
						break;
				        }
			        }
				fs_setmaskbit(block_index);
                		in.blocks[inode_block]=block_index;
				inode_block = inode_block + 1;
				fs_put_inode_by_num(0,in.id,&in);
				file_info.in = in;
            		}
            		else 
			{
				if(in.blocks[inode_block]>0)
				{
                			block_index = in.blocks[inode_block]; 
            			}
			}
            		if(num_bytes<(fsd.blocksz-inode_offset))
			{
                		bs_bwrite(0,block_index,inode_offset,buf,num_bytes);
                		file_info.fileptr = file_info.fileptr +  num_bytes;
                		num_bytes=0;
                		oft[fd] = file_info;
				return nbytes;
            		}
            		else
			{
                		bs_bwrite(0,block_index,inode_offset,buf,fsd.blocksz-inode_offset);
                		buf = buf + (fsd.blocksz-inode_offset);
                		file_info.fileptr = file_info.fileptr + (fsd.blocksz-inode_offset);
				num_bytes = num_bytes - (fsd.blocksz-inode_offset);
                		oft[fd] = file_info;
                		inode_offset=0;
            		}
       		     }
           }
	   else
	   {
		printf("File is closed\n");
		return SYSERR;
	   }
}
else
{
	printf("No file descriptor assigned\n");
	return SYSERR;
}
return nbytes;
}

#endif /* FS */
