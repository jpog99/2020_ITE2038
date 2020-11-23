#include "dbapi.hpp"
using namespace std;

int buf_get_tid(const char* pathname){
    int tid = open(pathname, O_CREAT | O_NOFOLLOW | O_RDWR | O_SYNC, 0666);; 
    if(tid == -1){
    	printf("File opening failed!\n");
    	return tid;
    }
    
    //copy the file header to buffer first frame
    string buf_header_idx = buf_add_page(tid,0);
    string pid = to_string(tid) + "." + to_string(0); 
    frame_t* buf_header = &(buf_pool.frames.find(pid)->second);
    //set the current mru to the newly added buffer
    adjust_mru_frame(buf_header_idx);
    
    int flag = read(tid, buf_header, PAGE_SIZE);
    buf_header->table_id = tid;
    buf_header->pnum = 0;
    buf_header->is_dirty = 0;
    buf_header->is_pinned = 0;
    if(!flag){
    	//memset(buf_header, 0, PAGE_SIZE);
    	buf_header->page.header_page.number_of_pages = 1;
    }
    buf_header->is_dirty = 1;
    buf_header->is_pinned = 0;
    return tid;
}

string buf_add_page(int tid, pagenum_t pnum){
    frame_t frame;
    string frame_idx;
    
    //case: all frame in buffer are in use
    if(buf_pool.frames.size() == buf_pool.size){
    	frame_idx = buf_pool.lru_frame;
    	//find frame index for eviction victim and remove it
    	while (buf_pool.frames[frame_idx].is_pinned)
            frame_idx = buf_pool.frames[frame_idx].lru_next;
        buf_remove_page(frame_idx);
        //frame = buf_pool.frames + frame_idx;
    }
    
    //set frame to the offset of frame_idx in buffer pool
    frame_idx = to_string(tid) + "." + to_string(pnum);
    memset(&frame, 0, PAGE_SIZE);
    frame.is_pinned = 0;
    frame.is_dirty = 0;
    frame.in_use = 1;
    frame.lru_next = "";
    frame.lru_prev = "";
    buf_pool.used_frame_cnt++;
    
    //insert new page into buffer pool
    buf_pool.frames.insert(make_pair(frame_idx, frame));
    return frame_idx;   	
}

int buf_remove_page(string frame_idx){
    frame_t* frame = &(buf_pool.frames.find(frame_idx)->second);
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
    if (frame->lru_next != "") 
        buf_pool.frames[frame->lru_next].lru_prev = frame->lru_prev;    
    else if (frame->lru_prev != "") 
        buf_pool.frames[frame->lru_prev].lru_next = frame->lru_next;
    
    //frame->in_use=0;
    //erase page from buffer
    buf_pool.frames.erase(buf_pool.frames.find(frame_idx));
    buf_pool.used_frame_cnt--;
    return SUCCESS;
}

frame_t* buf_read_page(int tid, pagenum_t pnum, int is_dirty){
    
    frame_t* frame;
    string frame_idx = to_string(tid) + "." + to_string(pnum);
    auto it = buf_pool.frames.find(frame_idx);
    
    //i = buf_pool.mru_frame;
    if (it != buf_pool.frames.end()) {
        frame = &(it->second);
        frame->is_pinned = 1;
    }
    else {
        frame_idx = buf_add_page(tid, pnum);
        frame = &(buf_pool.frames.find(frame_idx)->second);
        file_read_page(tid, pnum, frame);
    }
    
    frame->is_dirty |= is_dirty;
    adjust_mru_frame(frame_idx);
    return frame;
}    
    
void adjust_mru_frame(string frame_idx){
    //case: if current frame_idx is mru, keep mru as it is
    if (buf_pool.mru_frame == frame_idx) return;

    //case: current pool has no lru, then ser pool lru as current frame_idx
    if (buf_pool.lru_frame == "") 
        buf_pool.lru_frame = frame_idx;
    else if (buf_pool.lru_frame == frame_idx) 
        buf_pool.lru_frame = buf_pool.frames[frame_idx].lru_next;
    
    string next_frame_idx = buf_pool.frames[frame_idx].lru_next;
    string prev_frame_idx = buf_pool.frames[frame_idx].lru_prev;
    
    if (buf_pool.frames[frame_idx].lru_next != "")
        buf_pool.frames[next_frame_idx].lru_prev = buf_pool.frames[frame_idx].lru_prev;
    else if (buf_pool.frames[frame_idx].lru_prev != "") 
        buf_pool.frames[prev_frame_idx].lru_next = buf_pool.frames[frame_idx].lru_next;

    buf_pool.frames[frame_idx].lru_next = "";
    if (buf_pool.mru_frame != "") buf_pool.frames[buf_pool.mru_frame].lru_next = frame_idx;
    buf_pool.frames[frame_idx].lru_prev = buf_pool.mru_frame;
    buf_pool.mru_frame = frame_idx;
}
