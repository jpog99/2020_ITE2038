#ifndef __LOCK_TABLE_H__
#define __LOCK_TABLE_H__

#include <stdint.h>
#include <unordered_map>
#include <iostream>
#define SUCCESS 0;
#define FAILURE 1;
using namespace std;

typedef struct lock_t lock_t;

/* APIs for lock table */
int init_lock_table();
lock_t* lock_acquire(int table_id, int64_t key);
int lock_release(lock_t* lock_obj);

//hash function for hash_table
struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const pair<T1, T2>& p) const
    { 
        auto hash1 = hash<T1>{}(p.first); 
        auto hash2 = hash<T2>{}(p.second); 
        return hash1 ^ hash2; 
    } 
}; 

typedef struct pointer{
	lock_t* head;
	lock_t* tail;
}pointer;

#endif /* __LOCK_TABLE_H__ */
