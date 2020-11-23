#include "dbapi.h"
//#include "lock_manager.hpp"
// MAIN

int main( int argc, char ** argv ) {


    int64_t key;
    char instruction;
    char filename[MAX_FILENAME_LENGTH];
    int tid;
    //table_id_list tid_list[MAX_TABLE_COUNT];
    char value[VALUE_SIZE];
    int flag;
    int trx_id = trx_begin();

    init_db(BUFFER_SIZE);
    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 't':
               printf("======Current table_id list:=======\n");
        	for(int i=0 ; i<10 ; i++){
        	    printf("table_id:%d\nfilename:%s\n",tid_list[i].table_id,tid_list[i].pathname);
        	    printf("\n");
        	}
        	break;
        case 'o':
            scanf("%s",filename);
            tid = open_table(filename);
            break;
        case 'd':
            scanf("%d %ld",&tid, &key);
            flag = db_delete(tid, key);
            if(flag == SUCCESS) printf("Key %ld successfully deleted!\n", key);
            else printf("ERR: Deletion failed.\n");
            break;
        case 'i':
            scanf("%d %ld %s",&tid, &key,value);
            flag = db_insert(tid, key, value);
            if(flag == SUCCESS) printf("Key %ld (value: %s) successfully inserted!\n", key, value);
            else printf("ERR: Insertion failed.\n");
            break;
        case 'f':
            scanf("%d %ld",&tid, &key);
            flag = db_find(tid,key,value,trx_id);
            if(flag == SUCCESS) printf("Key: %ld\nValue: %s\n", key,value);
            else printf("ERR: Finding failed.\n");
            break;
        case 'u':
            scanf("%d %ld %s",&tid, &key,value);
            flag = db_update(tid,key,value,trx_id);
            if(flag == SUCCESS) printf("Key %ld (value: %s) successfully updated!\n", key, value);
            else printf("ERR: Update failed.\n");
            break;
        case 'c':
            scanf("%d",&tid);
            close_table(tid);
            break;    
        case 'q':
            while (getchar() != (int)'\n');
            shutdown_db();
            return EXIT_SUCCESS;
            break;
        default:
            break;
        }
        while (getchar() != (int)'\n');
        printf("> ");
    }
    printf("\n");

    return SUCCESS;
}
