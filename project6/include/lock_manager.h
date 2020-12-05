#pragma once

#include "dbapi.h"

//LOCK MANAGER APIs

//initialize global transaction table and return transaction id
int trx_begin(void);

//commit all changes to disk and end transaction, return parameter if success.
int trx_commit(int trx_id);

//initialize lock table
int init_lock_table(void);

//create new lock and insert the lock into lock table corresponding to parameter tid/key pair.
lock_t* lock_acquire(int table_id, int64_t key, int trx_id, int lock_mode);

//deadlock detection function (in test)
std::string find_deadlock(trx_t *trx1, trx_t *trx2);

//remove lock from lock table
int lock_release(lock_t* lock_obj);
