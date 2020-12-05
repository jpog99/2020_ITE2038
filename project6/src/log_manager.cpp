#include "dbapi.h"
using namespace std;

vector<log_t*>log_buf;
volatile int flushed_idx;
unordered_set<int> loser;
volatile int log_fd;
volatile int64_t last_LSN = 0;

int64_t create_log(int trx_id , int log_type){
    log_t* log = (log_t*)malloc(sizeof(log_t));
    log->prev_LSN = last_LSN;
    last_LSN += 40;
    log->LSN = last_LSN;
    log->trx_id = trx_id;
    log->log_type = log_type;
    log_buf.push_back(log);		
    return log->LSN;
}

void flush_log(){
    int log_buf_size = log_buf.size();
    log_t* log;
    
    for(flushed_idx; flushed_idx<log_buf_size ; flushed_idx++){
    	log = log_buf[flushed_idx];
    	if(log->log_type = UPDATE) write(log_fd, log, 280);
    	else write(log_fd, log, 40);
    }
}

int64_t create_log(int trx_id , int log_type, int tid, int pnum, int idx, char* old_image, char* new_image){
    log_t* log = (log_t*)malloc(sizeof(log_t));
    log->prev_LSN = last_LSN;
    last_LSN += 280;
    log->LSN = last_LSN;
    log->trx_id = trx_id;
    log->log_type = log_type;
    log->data_len = 120;
    log->tid = tid;
    log->pnum = pnum;
    log->offset = (idx+1)*128 + 8;
    
    memcpy(log->old_image, old_image, 120);
    memcpy(log->new_image, new_image, 120);
    
    log_buf.push_back(log);		
    return log->LSN;
}

int recover(char* log_path){
    log_fd = open(log_path, O_RDWR | O_SYNC | O_CREAT | O_APPEND, 0666);
	
    if (log_fd < 0) return FAILURE;
    
    log_t* log = (log_t*)malloc(sizeof(log_t));
    memset(log, 0, sizeof(log_t));
    while (pread(log_fd, log, 280, last_LSN) > 0) {
     	flushed_idx++;
     	switch(log->log_type){
     	    case BEGIN:
     	        loser.insert(log->trx_id);
                create_log(log->trx_id, BEGIN);
                break;
            
            case COMMIT:
                loser.erase(log->trx_id);
                create_log(log->trx_id, COMMIT);
                break;
                
            case ROLLBACK:
                create_log(log->trx_id, ROLLBACK);
                break;
            
            case UPDATE:
             	{
		        string filename = "DATA" + to_string(log->tid);
		        open_table(const_cast<char*>(filename.c_str()));
		        
		        string curr_idx = buf_add_page(log->tid, log->pnum);
		        int offset = log->offset;
		        int idx = (offset-8)/128 - 1;
		        int64_t LSN =  create_log(log->trx_id, log->log_type, log->tid, log->pnum, idx, log->old_image, log->new_image);
		        buf_pool.frames[curr_idx].is_pinned = 1;
		        page_t temp_page = buf_pool.frames[curr_idx].page;
		        if(temp_page.node_page.header.page_LSN < log->LSN){
		            temp_page.node_page.header.page_LSN = LSN;
			    memcpy(temp_page.node_page.records[idx].value, log->new_image, 120);
			    buf_pool.frames[curr_idx].is_dirty = 1;
		        }
		        buf_pool.frames[curr_idx].is_pinned = 0;
                }
                break;
            
            default:
                break; 
     	}
     }
     	
     	int size = log_buf.size();
     	for (int i = size-1; i>=0; i--) {
	    log_t* log = log_buf[i];
	    if (loser.count(log->trx_id)) {
		if (log->log_type == UPDATE) {
	        	int idx = ( (log->offset) - 8) / 128 - 1;
			create_log(log->trx_id, UPDATE, log->tid, log->pnum, idx, log->new_image, log->old_image);
			string curr_idx = buf_add_page(log->tid, log->pnum);
			buf_pool.frames[curr_idx].is_pinned = 1;
			page_t temp_page = buf_pool.frames[curr_idx].page;
			if (temp_page.node_page.header.page_LSN >= log->LSN) {
				temp_page.node_page.header.page_LSN = log->LSN - 1;
				memcpy(temp_page.node_page.records[idx].value, log->old_image, 120);
				buf_pool.frames[curr_idx].is_dirty = 1;
			}
			buf_pool.frames[curr_idx].is_pinned = 0;
		}
		else if (log->log_type == BEGIN) 
			loser.erase(log->trx_id);
	    }
        }
        
	free(log);
	return 0;
}

void print_log(){
	for (auto log : log_buf) {
	   	printf("LSN %lu [%d] Transaction id %d/n" , log->LSN, log->log_type, log->trx_id);
		if (log->log_type == UPDATE) {
			printf("OLD : %s, NEW : %s\n" , log->old_image, log->new_image);
		}
	}
}
