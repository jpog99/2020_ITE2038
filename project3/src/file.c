#include "file.h"
#include "bpt.h"
#include "buffer_manager.h"

frame_t* buf_alloc_page(int tid, frame_t* buf_header){
    //file_read_header();
    if(buf_header->page.header_page.free_page_no == 0){
        frame_t* new_page;
        int new_page_pnum = buf_add_page();
        new_page = buf_pool.frames + new_page_pnum;
        adjust_mru_frame(new_page_pnum);
        new_page->table_id = buf_header->table_id;
        new_page->page.free_page.next_free_page_pnum = buf_header->page.header_page.free_page_no;
        new_page->pnum = buf_header->page.header_page.number_of_pages;
        buf_header->page.header_page.free_page_no = new_page->pnum;
        buf_header->page.header_page.number_of_pages++;
        //file_write_page(new_page_pnum, new_page);
    }
    frame_t* temp_page;
    //file_read_page(pnum, temp_page);
    temp_page = buf_read_page(tid, buf_header->page.header_page.free_page_no, 1); 
    pagenum_t pnum = buf_header->page.header_page.free_page_no;
    buf_header->page.header_page.free_page_no = temp_page->page.free_page.next_free_page_pnum;
    return temp_page;
}

void buf_free_page(int tid, frame_t* buf_header, frame_t* page, pagenum_t pagenum){
    //page_t* page;
    //file_read_header();

    //page = (page_t*)malloc(sizeof(page_t));
    //file_read_page(pagenum,page);
    memset(page, 0, PAGE_SIZE);
    page->page.free_page.next_free_page_pnum = buf_header->page.header_page.free_page_no;
    buf_header->page.header_page.free_page_no = pagenum;

    //file_write_page(pagenum, page);
    //file_write_header();
    //free(page);
}

void file_read_page(int tid, pagenum_t pagenum, frame_t* dest){
    pread(tid, dest, PAGE_SIZE, pagenum*PAGE_SIZE);
    dest->table_id = tid;
    dest->pnum = pagenum;
    dest->is_pinned = 0;
    dest->is_dirty = 0;
}

void file_write_page(int tid, pagenum_t pagenum, const frame_t* src){
    pwrite(tid, src, PAGE_SIZE, pagenum*PAGE_SIZE);
}

frame_t* buf_read_header(int tid, int is_dirty){
    frame_t* buf_header;
    buf_header = buf_read_page(tid, 0, is_dirty);
    return buf_header;
}

