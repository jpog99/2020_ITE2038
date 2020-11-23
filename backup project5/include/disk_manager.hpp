#pragma once
#include "dbapi.hpp"

//FILE MANAGER APIs

// Allocate an on-disk page from the free page list
frame_t* buf_alloc_page(int tid, frame_t* buf_header);
// Free an on-disk page to the free page list
void buf_free_page(int tid, frame_t* buf_header, frame_t* page, pagenum_t pagenum);
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(int tid, pagenum_t pagenum, frame_t* dest);
// Write an in-memory page(src) to the on-disk page
void file_write_page(int tid, pagenum_t pagenum, const frame_t* src);
//Read header of db file
frame_t* buf_read_header(int tid, int is_dirty);

