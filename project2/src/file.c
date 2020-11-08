#include "file.h"
#include "bpt.h"

pagenum_t file_alloc_page(){
    file_read_header();
    if(main_header.header_page.free_page_no == 0){
        page_t* new_page;
        pagenum_t new_page_pnum = main_header.header_page.number_of_pages;
        memset(new_page,0,PAGE_SIZE);
        new_page->free_page.next_free_page_pnum = main_header.header_page.free_page_no;
        main_header.header_page.free_page_no = new_page_pnum;
        main_header.header_page.number_of_pages++;
        file_write_page(new_page_pnum, new_page);
    }
    page_t* temp_page;
    pagenum_t pnum = main_header.header_page.free_page_no;
    file_read_page(pnum, temp_page);
    main_header.header_page.free_page_no = temp_page->free_page.next_free_page_pnum;
    file_write_header();
    return pnum;
}

void file_free_page(pagenum_t pagenum){
    page_t* page;
    file_read_header();

    page = (page_t*)malloc(sizeof(page_t));
    file_read_page(pagenum,page);
    page->node_page.header.parent_pnum = main_header.header_page.free_page_no;
    main_header.header_page.free_page_no = pagenum;

    file_write_page(pagenum, page);
    file_write_header();
    free(page);
}

void file_read_page(pagenum_t pagenum, page_t* dest){
    pread(db, dest, PAGE_SIZE, pagenum*PAGE_SIZE);
}

void file_write_page(pagenum_t pagenum, const page_t* src){
    pwrite(db, src, PAGE_SIZE, pagenum*PAGE_SIZE);
}

void file_read_header( void ){
    pread(db, &main_header, PAGE_SIZE, 0);
}

void file_write_header( void ){
    pwrite(db, &main_header, PAGE_SIZE, 0);
}

