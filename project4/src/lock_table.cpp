#include <lock_table.h>
#include <pthread.h>

struct lock_t {
	/* NO PAIN, NO GAIN. */
	lock_t* prev;
	lock_t* next;
	unordered_map< pair<int,int> , pointer , hash_pair>::iterator sentinel_pointer;
	pthread_cond_t cond;
};

typedef struct lock_t lock_t;

unordered_map< pair<int,int> , pointer , hash_pair> hash_table_entry;
pthread_mutex_t lock_table_latch;

int
init_lock_table()
{
	/* DO IMPLEMENT YOUR ART !!!!! */
	lock_table_latch = PTHREAD_MUTEX_INITIALIZER;	
	return SUCCESS;
}

lock_t*
lock_acquire(int table_id, int64_t key)
{
	/* ENJOY CODING !!!! */
	pthread_mutex_lock(&lock_table_latch);

	lock_t* lock = (lock_t*)malloc(sizeof(lock_t));
	lock->prev = nullptr;
	lock->next = nullptr;
	lock->cond = PTHREAD_COND_INITIALIZER;
	pair<int,int> p(table_id,key);
	auto iter = hash_table_entry.find(p);

	//if hash not found, initialize new hash and set up lock list head and tail ptr
	if(iter == hash_table_entry.end()){
		hash_table_entry[p].head = lock;
		hash_table_entry[p].tail = lock->next;
		iter = hash_table_entry.find(p);
		lock->sentinel_pointer = iter;
	}
	
	//if hash already initialized
	else{
		lock->sentinel_pointer = iter;
		//case: if hash has no lock list
		if(iter->second.head == nullptr){
			iter->second.head = lock;
			iter->second.tail = lock->next;
		}

		else{
			//case: hash has only one lock in lock list, then make new lock as tail of lock list
			if(iter->second.head->next == nullptr){
				iter->second.tail = lock;
				iter->second.head->next = lock;
			}

			//case: hash has a lock list, then append new lock at tail
			else{
				iter->second.tail->next = lock;
				iter->second.tail = lock;
			}

			//sleep while the new lock is not the head of lock list  
			while(lock != iter->second.head){
				pthread_cond_wait(&lock->cond, &lock_table_latch);
			}
		}
	}


	pthread_mutex_unlock(&lock_table_latch);
	return lock;
}

int
lock_release(lock_t* lock_obj)
{
	/* GOOD LUCK !!! */
	pthread_mutex_lock(&lock_table_latch);

        auto iter = lock_obj->sentinel_pointer;
	//case failure: the program trying to release non-head lock
	if(lock_obj != iter->second.head)
		return FAILURE;
	
	//case: only one lock exist in lock list, then nullify head and tail pointer
	if(lock_obj->next == nullptr){
		iter->second.head = nullptr;
		iter->second.tail = nullptr;
	}

	//case: lock_obj has a successor, then signal the successor and make it as head of lock list
	if(lock_obj->next != nullptr){
		iter->second.head = lock_obj->next;
		iter->second.head->prev = nullptr;
		
		//wake up the next lock of lock_obj
		pthread_cond_signal(&lock_obj->next->cond);
	}

	free(lock_obj);
	pthread_mutex_unlock(&lock_table_latch);
	return 0;
}
