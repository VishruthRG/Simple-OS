/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------
 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.
 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_baseFrameNo, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/
// head pointer to the ContFramePool structure
ContFramePool* ContFramePool::head = NULL; 

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

ContFramePool::ContFramePool(unsigned long _baseFrameNo,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{
    	baseFrameNo = _baseFrameNo;
	nFrames = _n_frames;
	nFreeFrames = _n_frames;
	info_frame_no = _info_frame_no;
	ninfoframes = _n_info_frames;
	
/* If _info_frame_no is zero then we keep management info in the first
   frame, else we use the provided frame to keep management info */
   
   
    if(info_frame_no == 0) {
        bitmap = (unsigned char *) (baseFrameNo * FRAME_SIZE);
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }
    
    /* Number of frames should be able to "fill" the bitmap!*/
    assert ((nFrames % 8 ) == 0);
        
    /* Initializing all bits in the bitmap. Using 8 bits to represent the
	one frame, in which first two bits are used to store information regarding 
	allocation and Head*/
    for(int i=0; i < _n_frames; i++) {
        bitmap[i] = 0xFF;
    }
    
    /* To Mark the first Frame as being used for Kernel Pool */
    if(_info_frame_no == 0) {
        bitmap[0] = 0x7F;
        nFreeFrames--;		
    }
	
	
    if(head == NULL){
        head = this;   	 
        head->next = NULL;
    }
    else{
        ContFramePool *temp = head;
        while (temp->next != NULL) { //traversing the list
        	temp = temp->next;
        }
        temp->next = this; //add newest frame pool to the end of the list
        temp = this;
        temp->next = NULL;
    }	
    
    Console::puts("Frame Pool initialized\n");
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames) {
    unsigned int i = 0, j = 0;
    unsigned int startFrameNo = baseFrameNo; 
    unsigned int frameCount;	
    // Checking range of frames
    if (_n_frames > nFreeFrames || _n_frames > nFrames) {
    	Console::puts("Memory not available!\n");
    	return 0;
    }
		   
    lookup:
    while (bitmap[i] == 0x7F || bitmap[i] == 0x3F) { //0x7F is allocated and 0x3F is head of sequence
		i++;
    }
	
    startFrameNo = baseFrameNo + i;
	
    for(j = i; j < (_n_frames + i); j++) {
    	// out of bounds condition
        if (startFrameNo + _n_frames > baseFrameNo + nFrames) {
	    Console::puts("Contiguous memory frames not available\n");
	    return 0;
	}
		
	if((bitmap[j] == 0x7F) || (bitmap[j] == 0x3F)) {
	    if(j < baseFrameNo+nFrames) {
                i = j;
                goto lookup; 		   
	    } else {
                Console::puts("No continuous location available");
		 return 0;
	    }			
		  
	}
	
    }  
    	frameCount = 1;
	while (frameCount <= _n_frames) {
		if (frameCount == 1) {
			bitmap[i] ^= 0xC0; // mark 00 for head of sequence
		} else {
			bitmap[i] ^= 0x80; // mark 01 for allocated but not head of sequence
		}
		nFreeFrames--;
		frameCount++;
		i++;
	}
	
	return (startFrameNo);
}

// mark_accessible very similar to that for simple frame manager, only difference is to mark each frame in the _n_frames range as allocated with first two bits 00

void ContFramePool::mark_inaccessible(unsigned long _baseFrameNo,
                                      unsigned long _n_frames) {   
    int i ;
    for(i = _baseFrameNo; i < _baseFrameNo + _n_frames; i++){
	mark_inaccessible(i);		
    }
    nFreeFrames -= _n_frames;
}

void ContFramePool::mark_inaccessible(unsigned long _frame_no) {
	if (_frame_no < baseFrameNo || _frame_no > (baseFrameNo + nFrames)) {
		Console::puts("Out of range\n");
		return;
	}
    
    unsigned int bitmap_index = _frame_no - baseFrameNo;
    unsigned char mask = 0xC0;
    
    assert((bitmap[bitmap_index] & mask) != 0); 
    
    // Update bitmap
    bitmap[bitmap_index] ^= mask;	
}



void ContFramePool::release_frames(unsigned long _first_frame_no) {
	ContFramePool *temp = head;
	while (temp != NULL) { // this while loop ensures release of a frame from within frame pool
		if (_first_frame_no >= temp->baseFrameNo && _first_frame_no <= temp->baseFrameNo + temp->nFrames -1) {
			// if this condition passes that means the frame is within the bounds of the allocated frames
			temp->release_frame(_first_frame_no);
		}
		temp = temp->next;
	}
}

void ContFramePool::release_frame(unsigned long _first_frame_no ) {

	if(bitmap[_first_frame_no - baseFrameNo] != 0x3F) {
		Console::puts("Frame allocated but not head of sequence, cannot remove\n");
		return;	
	}

	unsigned int i = (_first_frame_no - baseFrameNo);
	bitmap[i] = 0xFF; // resetting head of sequence to free
	i++;
	nFreeFrames++;
	while (i < nFrames && i ^ 0xFF == 0x80) { // loop for the remaining contiguous allocated frames
		if (bitmap[i] ^ 0xFF == 0x80) { // this condition when first two bits are 01 i.e allocated
			bitmap[i] = 0xFF;
		}
		nFreeFrames++;
		i++;
	}
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames) {
    // using 8 bits per frame
   return ((_n_frames * 8)/(4*1024)) + (((_n_frames * 8) % (4 * 1024)) >0 ? 1 : 0);
}

