/*
 File: vm_pool.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
    // assert(false);
    
    // initialize variables
    unsigned long baseAddress = _base_address;
    unsigned long size = _size;
    ContFramePool * framePool = _frame_pool;
    PageTable * pageTable = _page_table;
    vmPool_next = NULL; // next pointer to the vmPool list
    countRegions = 0; // count of memory regions
    
    pageTable->register_pool(this); // registering current (initialized) pool
    
    memRegion_DS * region = (memRegion_DS *)baseAddress;
    region[0].memBaseAddress = baseAddress;
    region[0].memSize = PageTable::PAGE_SIZE;
    memRegion = region;
    countRegions = countRegions + 1;
    availableSize = availableSize - PageTable::PAGE_SIZE;
    
    
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
    // assert(false);
    if(_size > availableSize){ // memory size check
    	Console::puts("Not enough memory size available to allocate VM Pool\n");
    	return 0;
    }
    unsigned long requestedPages = (_size / PageTable::PAGE_SIZE) + (_size % PageTable::PAGE_SIZE > 0 ? 1 : 0);
    memRegion[countRegions].memBaseAddress = memRegion[countRegions - 1].memBaseAddress + memRegion[countRegions - 1].memSize; // calculate new base address as previous base address + previous size
    memRegion[countRegions].memSize = requestedPages * PageTable::PAGE_SIZE;
    countRegions = countRegions + 1;
    availableSize = availableSize - (requestedPages * PageTable::PAGE_SIZE);
    
    
    Console::puts("Allocated region of memory.\n");
    return memRegion[countRegions - 1].memBaseAddress;
}

void VMPool::release(unsigned long _start_address) {
    // assert(false);
    int i = 0;
    unsigned long regionToRelease = -1; 
    while (i < countRegions) { 
    	if (memRegion[i].memBaseAddress == _start_address) { // look for the region whose address is the given start address to releqase
    	    regionToRelease = i;
    	}
    }
    
    if (regionToRelease == -1) { // case where region is not found
        Console::puts("Region to release not found in vm pool\n");
    }
    
    unsigned long pagesToRelease = memRegion[regionToRelease].memSize / PageTable::PAGE_SIZE; 
    while(pagesToRelease > 0) { // release the necessary number of pages
    	pageTable->free_page(_start_address);
    	pagesToRelease = pagesToRelease - 1;
    	_start_address = _start_address + PageTable::PAGE_SIZE;
    }
    
    unsigned long r = regionToRelease;
    while(r < countRegions){ // remove region data 
    	memRegion[r] = memRegion[r+1];
   	r = r + 1;
    }
    
    // updating the vm pool data
    countRegions = countRegions - 1;
    availableSize = availableSize + memRegion[regionToRelease].memSize;
    
    Console::puts("Released region of memory.\n");
}

bool VMPool::is_legitimate(unsigned long _address) {
    //assert(false);
    if ((_address < baseAddress) or (_address > (baseAddress + size))) { // out of bounds cases
    	return false;
    }
    return true;
    // Console::puts("Checked whether address is part of an allocated region.\n");
}

