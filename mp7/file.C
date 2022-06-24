/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"
extern FileSystem * fs;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id) {
    Console::puts("Opening file.\n");
    curr_block = 2; // block 0 holds inode list and block 1 holds free list
    file_pos = 0;
    FileSystem *fs = _fs;
    //assert(false);
}

File::~File() {
    Console::puts("Closing file.\n");
    /* Make sure that you write any cached data to disk. */
    fs->disk->write(curr_block, (unsigned char *)block_cache);
    /* Also make sure that the inode in the inode list is updated. */
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
    Console::puts("reading from file\n");
    int to_read = _n;
    //int read = 0;
    int char_count = 0;
    fs->disk->read(curr_block, (unsigned char *)block_cache);
    while (!EoF() && to_read > 0) {
    	_buf[char_count] = block_cache[file_pos];
    	to_read = to_read - 1;
    	char_count = char_count + 1;
    	file_pos = file_pos + 1;
    }
    curr_block = curr_block + 1;
    return char_count;
    //assert(false);
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");
    int to_write = _n;
    //int written = 0;
    int char_count = 0;
    char write_buf[512];
    fs->disk->read(curr_block, (unsigned char *)block_cache);
    while(!EoF() && to_write > 0) {
    	block_cache[file_pos] = _buf[char_count];
    	to_write = to_write - 1;
    	char_count = char_count + 1;
    }
    curr_block = curr_block + 1;
    fs->disk->write(curr_block, (unsigned char *)block_cache);
    return char_count;
    //assert(false);  
}

void File::Reset() {
    Console::puts("resetting file\n");
    file_pos = 0;
    //assert(false);
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    if (file_pos > 512) {
    	return true;
    }
    return false;
    //assert(false);
}
