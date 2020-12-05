#include "dbapi.h"
using namespace std;

pthread_mutex_t lock_mutex;
pthread_mutex_t trx_mutex = PTHREAD_MUTEX_INITIALIZER;
list<trx_t> trx_list;
unordered_map<string, lock_t*> lock_table;

int trx_begin(void){
    trx_t trx;
    pthread_mutex_t trx_mutex = PTHREAD_MUTEX_INITIALIZER;
    int max = 1;
    trx.trx_id = __sync_fetch_and_add(&max, 1);
    trx.state = "IDLE";
    
    //lock global trx mutex
    pthread_mutex_lock(&trx_mutex);
    //add new trx into trx_list 
    trx_list.push_back(trx);
    create_log(trx.trx_id, BEGIN);
    return trx.trx_id;
}

int trx_commit(int trx_id){
    list<trx_t>::iterator it;
    for (it = trx_list.begin(); it != trx_list.end(); ++it){
    	if(it->trx_id == trx_id) break;
    }
  
    if (it == trx_list.end()) return 0;
   
    int lock_count = it->lock_list.size();
    for (int i = 0; i < lock_count; i++) {
        lock_release(*it->lock_list.begin());
    }
    
    //remove trx from trx_list
    trx_list.erase(it);
    //create log commit
    create_log(trx_id,COMMIT);
    //unlock current trx mutex
    pthread_mutex_unlock(&trx_mutex);

    return trx_id;
}

int trx_abort(int trx_id){
    create_log(trx_id, ROLLBACK);
    
    //rollback process
    int log_size = log_buf.size();
    for(int i = log_size-1 ; i>=0 ; i--){
    	log_t* log = log_buf[i];
    	if(log->trx_id == trx_id) {
    	    if(log->log_type == UPDATE) {
    	        int idx = ( (log->offset)-8)/128 - 1;
    	        create_log(trx_id, UPDATE, log->tid, log->pnum, idx, log->new_image, log->old_image);
    	        string curr_idx = buf_add_page(log->tid, log->pnum);
    	        buf_pool.frames[curr_idx].is_pinned=1;
    	        page_t temp_page = buf_pool.frames[curr_idx].page;
    	        buf_pool.frames[curr_idx].is_dirty = 1;
    	        memcpy(temp_page.node_page.records[i].value, log->old_image, 120);
    	        buf_pool.frames[curr_idx].is_pinned = 0;
    	    }
    	    else if(log->log_type == BEGIN) break;
    	}
    }
    
    return trx_commit(trx_id);
}
 

int init_lock_table(void){
    lock_mutex = PTHREAD_MUTEX_INITIALIZER;
    return SUCCESS;
}

lock_t* lock_acquire(int table_id, int64_t key, int trx_id, int lock_mode){
    lock_t* lock = new lock_t;
    lock->tid = table_id;
    lock->key = key;
    lock->lock_mode = lock_mode;
    
    list<trx_t>::iterator it;
    for (it = trx_list.begin(); it != trx_list.end(); ++it){
    	if(it->trx_id == trx_id) break;
    }
  
    if (it == trx_list.end()) return NULL;
    trx_t* trx = &(*it);
    lock->trx = trx;

    //lock the new key in lock table
    pthread_mutex_lock(&lock_mutex);

    //insert lock in lock list
    trx->lock_list.push_back(lock);
    string pid = to_string(table_id) + "." + to_string(key);
    lock_table.insert(make_pair(pid,lock));
    
    return lock;
}

int lock_release(lock_t* lock_obj){
    list<lock_t*>* trx_lock_list = &(lock_obj->trx->lock_list);
    trx_lock_list->erase(find(trx_lock_list->begin(), trx_lock_list->end(), lock_obj));
    
    string pid = to_string(lock_obj->tid) + "." + to_string(lock_obj->key);
    lock_table.erase(pid);
    
    //unlock lock mutex
    pthread_mutex_unlock(&lock_mutex);
    
    delete lock_obj;

    return SUCCESS;
}
