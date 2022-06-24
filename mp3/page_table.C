#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
   // assert(false);
   kernel_mem_pool = _kernel_mem_pool;
   process_mem_pool = _process_mem_pool;
   const unsigned long sharedSize = _shared_size;
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   // assert(false);
   page_directory = (unsigned long *)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);
   unsigned long * pageTable = (unsigned long *)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);

   unsigned int address = 0;
   unsigned int i;

   // loop over all page table entries and mark each as being present
   for(i = 0; i < 1024; i++) {
      pageTable[i] = address | 3;
      address = address + 4096;
   }

   // mark first page directory entry as present
   page_directory[0] = (unsigned long)pageTable;
   page_directory[0] = (page_directory[0] | 3);

   // mark all other page directory entries as not present
   for(i = 1; i < 1024; i++) {
      page_directory[i] = 0 | 2;
   }
   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
   // assert(false);
   current_page_table = this; // store current page table address
   write_cr3((unsigned long)page_directory); // write into cr3 register the address of the page directory
   
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   // assert(false);
   
   PageTable::paging_enabled = 1; // mark paging as enabled
   write_cr0(read_cr0() | 0x80000000); // write into the cr0 register
   
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  // assert(false);
  unsigned long faulty_address = read_cr2(); // determine faulty address from cr2 register
  unsigned long* page_dir = current_page_table->page_directory; //retrieve pointer to page directory
  
  unsigned long pg_dir_index = faulty_address >> 22; // retrieve first 10 bits of faulty address which is the page directory index
  unsigned long pg_table_index = (faulty_address & 0x003FF000 ) >> 12; // retrieve middle 10 bits which is page table index
  
  unsigned long *page_table; 
  unsigned long *page_table_entry; 
  
  if((page_dir[pg_dir_index] & 1) == 0) { // checks if present bit is not set
  
  // if present bit not set, it means page directory entry does not exist
  	page_table = (unsigned long *)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);
  	page_dir[pg_dir_index] = (unsigned long)page_table; // add new page table into the page directory at the corresponding index
  	page_dir[pg_dir_index] = page_dir[pg_dir_index] | 3; // set present bit as 1
  	
  	for (unsigned long int i = 0; i < ENTRIES_PER_PAGE; i++) {
  		page_table[i] = 2;
  	} // setting all page table entries to be not present with last bit 0
  }
  
  page_table = (unsigned long *)(page_dir[pg_dir_index] & ~0xFFF); // retrieve page table address
  
  if ((page_table[pg_table_index] & 1) == 0) { // checks if present bit is not set
  // if present bit not set then page table entry does not exist
  	page_table_entry = (unsigned long *)(process_mem_pool->get_frames(1) * PAGE_SIZE);
  	page_table[pg_table_index] = (unsigned long)page_table_entry; // add new page table entry into the page table at corresponding index
  	page_table[pg_table_index] = page_table[pg_table_index] | 3; // set present bit 

  }
  
  Console::puts("handled page fault\n");
}

