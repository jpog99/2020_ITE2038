#include "buffer_manager.h"

int buf_get_tid(const char* pathname){
    int tid = open(pathname, O_CREAT | O_NOFOLLOW | O_RDWR | O_SYNC, 0666);; 
    if(tid == -1){
    	printf("File opening failed!\n");
    	return tid;
    }
    
    //copy the file header to buffer first frame
    int buf_header_idx = buf_add_page();
    frame_t* buf_header = buf_pool.frames + buf_header_idx;
    //set the current mru to the newly added buffer
    adjust_mru_frame(buf_header_idx);
    
    int flag = read(tid, buf_header, PAGE_SIZE);
    buf_header->table_id = tid;
    buf_header->pnum = 0;
    buf_header->is_dirty = 0;
    buf_header->is_pinned = 0;
    if(!flag){
    	memset(buf_header, 0, PAGE_SIZE);
    	buf_header->page.header_page.number_of_pages = 1;
    }
    buf_header->is_dirty = 1;
    return tid;
}

int buf_add_page(){
    frame_t* frame;
    int frame_idx;
    
    //case: free frame available in buffer
    if(buf_pool.used_frame_cnt != buf_pool.size){
    	for(int i=0 ; i<buf_pool.size ; i++)
    	    if(!buf_pool.frames[i].in_use) frame_idx=i;
    	frame = buf_pool.frames + buf_pool.used_frame_cnt;
    }
    
    //case: all frame in buffer are in use
    else{
    	frame_idx = buf_pool.lru_frame;
    	//find frame index for eviction victim and remove it
    	while (buf_pool.frames[frame_idx].is_pinned)
            frame_idx = buf_pool.frames[frame_idx].lru_next;
        buf_remove_page(frame_idx);
        frame = buf_pool.frames + frame_idx;
    }
    //set frame to the offset of frame_idx in buffer pool
    frame = buf_pool.frames + frame_idx;
    frame->is_pinned = 0;
    frame->is_dirty = 0;
    frame->in_use = 1;
    frame->lru_next = -1;
    frame->lru_prev = -1;
    buf_pool.used_frame_cnt++;
    
    return frame_idx;   	
}

int buf_remove_page(int frame_idx){
    frame_t* frame = buf_pool.frames + frame_idx;
    frame->is_pinned = 1;
    
    //write to disk first if dirty
    if(frame->is_dirty) file_write_page(frame->table_id, frame->pnum, frame);
    
    //case:if deleting frame is lru, set the next frame as lru frame in pool.
    if(buf_pool.lru_frame == frame_idx) 
    	buf_pool.lru_frame = frame->lru_next;
    //case:if deleting frame is mru, set the prev frame as mru frame in pool.
    else if(buf_pool.mru_frame == frame_idx) 
    	buf_pool.mru_frame = frame->lru_prev;
    	
    //case: when this frame has either next or prev frame
    if (frame->lru_next != -1) 
        buf_pool.frames[frame->lru_next].lru_prev = frame->lru_prev;    
    else if (frame->lru_prev != -1) 
        buf_pool.frames[frame->lru_prev].lru_next = frame->lru_next;
    
    frame->in_use=0;
    buf_pool.used_frame_cnt--;
    return SUCCESS;
}

frame_t* buf_read_page(int tid, pagenum_t pnum, int is_dirty){
    
    frame_t* frame;
    int frame_idx, i;
    //case: all frames in pool are free
    if(buf_pool.used_frame_cnt == 0) i = -1;
    
    i = buf_pool.mru_frame;
    frame_t* temp_frame = buf_pool.frames + i;
    while(temp_frame->lru_prev != -1){
    	if(temp_frame->table_id == tid && temp_frame->pnum == pnum) frame_idx == i;
    	i = temp_frame->lru_prev;
    	temp_frame = buf_pool.frames + i;
    }
    if(temp_frame->table_id == tid && temp_frame->pnum == pnum) frame_idx = i;
    else frame_idx = -1;
    
    if(frame_idx == -1){
    	frame_idx = buf_add_page();
    	frame = buf_pool.frames + frame_idx;
    	file_read_page(tid, pnum, frame);
    }else 
        frame = buf_pool.frames + frame_idx;
    frame->is_pinned = 1;
    frame->is_dirty |= is_dirty;
    adjust_mru_frame(frame_idx);
    return frame;
}    
    
void adjust_mru_frame(int frame_idx){
    //case: if current frame_idx is mru, keep mru as it is
    if (buf_pool.mru_frame == frame_idx) return;

    //case: current pool has no lru, then ser pool lru as current frame_idx
    if (buf_pool.lru_frame == -1) 
        buf_pool.lru_frame = frame_idx;
    else if (buf_pool.lru_frame == frame_idx) 
        buf_pool.lru_frame = buf_pool.frames[frame_idx].lru_next;
    
    int next_frame_idx = buf_pool.frames[frame_idx].lru_next;
    int prev_frame_idx = buf_pool.frames[frame_idx].lru_prev;
    
    if (buf_pool.frames[frame_idx].lru_next != -1)
        buf_pool.frames[next_frame_idx].lru_prev = buf_pool.frames[frame_idx].lru_prev;
    else if (buf_pool.frames[frame_idx].lru_prev != -1) 
        buf_pool.frames[prev_frame_idx].lru_next = buf_pool.frames[frame_idx].lru_next;

    buf_pool.frames[frame_idx].lru_next = -1;
    if (buf_pool.mru_frame != -1) buf_pool.frames[buf_pool.mru_frame].lru_next = frame_idx;
    buf_pool.frames[frame_idx].lru_prev = buf_pool.mru_frame;
    buf_pool.mru_frame = frame_idx;
}


    
    
    

