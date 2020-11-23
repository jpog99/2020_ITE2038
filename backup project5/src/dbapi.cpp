//#include "file.hpp"
#include "dbapi.hpp"
//#include "buffer_manager.hpp"
using namespace std;

int init_db(int buf_num){
    if(buf_exist == 1){
    	printf("Buffer already exist!\n");
    	return FAILURE;
    }else if(buf_num <=0){
     	printf("Buffer size must be more than 0!\n");
     	return FAILURE;
    }
    
    buf_exist = 1;
    
    //buf_pool.frames = (frame_t*)calloc(buf_num, sizeof(frame_t));
    buf_pool.size = buf_num;
    buf_pool.used_frame_cnt = 0;
    buf_pool.mru_frame = "";
    buf_pool.lru_frame = "";
    
    for(int i=0 ; i<10 ; i++)
    	tid_list[i].table_id = 0;
    	
    return SUCCESS;
    
}


int open_table(const char* pathname){
    if(pathname == NULL){
        printf("Null file name!\n");
        return -1;
    }
    
    if(strlen(pathname) > MAX_FILENAME_LENGTH){
         printf("ERR: Filename must be less than 20 characters long!\n");
         return -1;
    }

   int i;
   for(i=0 ; i<10 ; i++){
   	if (tid_list[i].table_id == 0) break;
   	if(!strcmp(tid_list[i].pathname, pathname)){
   	    printf("File already opened!\n");
   	    return -1;
   	}
   }
   
   
   if (i==10){
   	printf("Table id list is full!\n");
   	return -1;
   }

   	
    tid_list[i].table_id = buf_get_tid(pathname);
    strcpy(tid_list[i].pathname, pathname);
    printf("open_table successful with table_id:%d\n",tid_list[i].table_id);
    
    return tid_list[i].table_id;
}

int db_insert(int tid, int64_t key, char* value){
    if(tid<=0){
    	printf("No table currently opened!\n");
    	return FAILURE;
    }
    
    int i;
    for(i=0 ; i<MAX_TABLE_COUNT ; i++){
    	if(tid_list[i].table_id == tid)
    	   return insert(tid, key, value);
    }
    
    if(i==MAX_TABLE_COUNT){
    	printf("No matching table id!\n");
    	return FAILURE;
    }
}

int db_delete(int tid, int64_t key){
    if(tid<=0){
    	printf("No table currently opened!\n");
    	return FAILURE;
    }
    
    int i;
    for(i=0 ; i<MAX_TABLE_COUNT ; i++){
    	if(tid_list[i].table_id == tid)
    	   return deletion(tid, key);
    }
    
    if(i==MAX_TABLE_COUNT){
    	printf("No matching table id!\n");
    	return FAILURE;
    }
}

int db_find(int tid, int64_t key, char* ret_val, int trx_id){
    if(tid<=0){
    	printf("No table currently opened!\n");
    	return FAILURE;
    }
    int i;
    for(i=0 ; i<MAX_TABLE_COUNT ; i++){
    	if(tid_list[i].table_id == tid){
    	   return find(tid, key, ret_val, trx_id);break;
    	}
    }
    
    if(i==MAX_TABLE_COUNT){
    	printf("No matching table id!\n");
    	return FAILURE;
    }
}

int db_update(int table_id, int64_t key, char* values, int trx_id){
    if(tid<=0){
    	printf("No table currently opened!\n");
    	return FAILURE;
    }
    int i;
    for(i=0 ; i<MAX_TABLE_COUNT ; i++){
    	if(tid_list[i].table_id == tid){
    	   return update(tid, key, values, trx_id);break;
    	}
    }
    
    if(i==MAX_TABLE_COUNT){
    	printf("No matching table id!\n");
    	return FAILURE;
    }
}
int close_table(int table_id){
    if(table_id<=0){
    	printf("No table opened or invalid table id!\n");
    	return FAILURE;
    }
    //search if table_id exist
    int i;
    for(i=0 ; i<MAX_TABLE_COUNT ; i++){
    	if(tid_list[i].table_id == table_id) break;
    }
    
    if(i==MAX_TABLE_COUNT){
    	printf("No such table id!\n");
    	return FAILURE;
    }

    frame_t* last_frame;
    //last_frame = buf_pool.frames + buf_pool.lru_frame;
    string last_frame_idx = buf_pool.lru_frame;
    if(last_frame_idx == "") return 0;
    last_frame = &(buf_pool.frames.find(last_frame_idx)->second);
    
    //save all changes to disk from lru frame one by one and remove the frame
    while(last_frame->lru_next !=""){
    	string next_frame_idx = last_frame->lru_next;
    	if(last_frame->table_id == table_id)
    	    buf_remove_page(last_frame_idx);
    	last_frame_idx = next_frame_idx;
    	last_frame = &(buf_pool.frames.find(last_frame_idx)->second);
    }
    //case when there's one a single page in buffer pool
    if(last_frame->table_id == table_id)
        buf_remove_page(last_frame_idx);
        
    //update table id list
    for(i=0 ; i<MAX_TABLE_COUNT ; i++){
    	if(tid_list[i].table_id == table_id){
    	    tid_list[i].table_id = 0;
    	    strcpy(tid_list[i].pathname, "");
    	    break;
    	}
    }
    
    printf("Table %d successfully closed!. All changes saved.\n",table_id);
    
    return SUCCESS;
}

int shutdown_db(){
    if(buf_exist == 0) return FAILURE;
    
    //basically same as close_table() but this time ignore table id checking
    string last_frame_idx = buf_pool.lru_frame;
    if(last_frame_idx == "") return 0;
    
    //frame_t* last_frame = buf_pool.frames + buf_pool.lru_frame;
    frame_t* last_frame = &(buf_pool.frames.find(last_frame_idx)->second);
    
    while (last_frame->lru_next != ""){
    	string next_frame_idx = last_frame->lru_next;
    	buf_remove_page(last_frame_idx);
    	last_frame_idx = next_frame_idx;
    	last_frame = &(buf_pool.frames.find(next_frame_idx)->second);;
    }
    buf_remove_page(last_frame_idx);
    //free(buf_pool.frames);
    buf_exist = 0;
    
    printf("All changes saved!\n");
    return SUCCESS;
}
