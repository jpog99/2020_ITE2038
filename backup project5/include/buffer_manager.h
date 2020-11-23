#pragma once

#include "file.h"


//BUFFER MANAGER APIs

//fetch a page from disk and add it to buffer pool. returns tid/pnum pair in string.
std::string buf_add_page(int tid, pagenum_t pnum);

//write changes to disk and remove page from buffer pool corresponding to frame index in parameter.
int buf_remove_page(std::string frame_idx);

//set the mru frame in the buffer pool everytime a frame is used in upper layer.
void adjust_mru_frame(std::string frame_idx);

//read the header of the disk into buffer pool and returns table_id.
int buf_get_tid(const char* pathname);

//function used by the top layer to read page in buffer pool.
frame_t* buf_read_page(int tid, pagenum_t pnum, int is_dirty);
