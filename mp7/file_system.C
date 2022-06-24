/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"
//extern FileSystem *fs;
/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */

/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
    //assert(false);
    FileSystem * fs;
    FileSystem::disk = NULL;
    size = 0;    
    FileSystem::free_blocks[512];
    FileSystem::inodes[MAX_INODES];
}

FileSystem::~FileSystem() {
    Console::puts("unmounting file system\n");
    /* Make sure that the inode list and the free list are saved. */
    assert(false);
}



/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system from disk\n");
    disk = _disk;
    /* Here you read the inode list and the free list into memory */
    
    //assert(false);
    return true;
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
    Console::puts("formatting disk\n");
    /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
     
     FileSystem * fs;
    _disk->write(0, (unsigned char *)fs->inodes);
    _disk->write(1, (unsigned char *)fs->free_blocks);
    //assert(false);
    return true;
}

Inode * FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");
    /* Here you go through the inode list to find the file. */
    //assert(false);
    FileSystem * fs;
    Inode * new_inode = fs->inodes;
    while(new_inode->next_inode != NULL) {
    	if (new_inode->id == _file_id) {
    	  return new_inode;
    	}
    }
    return NULL;
}

bool FileSystem::CreateFile(int _file_id) {
    Console::puts("creating file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* Here you check if the file exists already. If so, throw an error.
       Then get yourself a free inode and initialize all the data needed for the
       new file. After this function there will be a new file on disk. */
       FileSystem *fs;
    if (LookupFile(_file_id)) {
    	Console::puts("File exists already\n");
    	return false;
    }
    Inode * new_inode = new Inode;
    new_inode->id = _file_id;
    new_inode->next_inode = NULL;
    Inode * in_ptr = fs->inodes;
    while (in_ptr->next_inode != NULL) {
    	in_ptr = in_ptr->next_inode;
    }
    
    in_ptr->next_inode = new_inode;
    return true;
    //assert(false);
}

bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* First, check if the file exists. If not, throw an error. 
       Then free all blocks that belong to the file and delete/invalidate 
       (depending on your implementation of the inode list) the inode. */
      Inode * in_ptr;
      while (in_ptr->next_inode != NULL) {
      	  if (in_ptr->id == _file_id) {
      	    in_ptr->next_inode = in_ptr->next_inode->next_inode;
      	    return true;
      	  }
          in_ptr = in_ptr->next_inode;
      }
      
      return false;
}
