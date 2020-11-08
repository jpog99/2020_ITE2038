#include "file.h"
#include "dbapi.h"


int open_table(const char* pathname){
    if(pathname == NULL){
        printf("Null file name!\n");
        return FAILURE;
    }

    db = open(pathname, O_CREAT | O_RDWR | O_SYNC, 0777);
    if(db < 0) {
        printf("Error while opening file!\n");
        return FAILURE;
    }
    if(read(db,&main_header,PAGE_SIZE) == 0){
        memset(&main_header,0,PAGE_SIZE);
        main_header.header_page.number_of_pages = 1;
        file_write_header();
    }
    return db;
}

int db_insert(int64_t key, char* value){
    return insert(key, value);
}

int db_delete(int64_t key){
    return delete(key);
}

int db_find(int64_t key, char* ret_val){
    return find(key,ret_val);
}

