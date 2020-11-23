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
    //unlock current trx mutex
    pthread_mutex_unlock(&trx_mutex);

    return trx_id;
}

//ausba dlm test
//testing deadlock detection function
string find_deadlock(trx_t* trx1, trx_t* trx2) {

    trx_t* curr_trx = trx2;
    lock_t lock;
    list<lock_t>::iterator it;
    //for (it = curr_trx->lock_list.begin(); it != curr_trx->lock_list.end(); ++it){
    //	if(it == trx1->lock_list.begin()) break;
    //}
  
    while (curr_trx->state== "WAITING" && curr_trx != trx1) {
        //lock = *it;
        curr_trx = lock.trx;
    }

    if (curr_trx == trx1) {
        return "DEADLOCK";
    } else {
        return "SAFELOCK";
    }
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
