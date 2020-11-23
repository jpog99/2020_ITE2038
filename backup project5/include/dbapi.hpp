#ifndef DBAPI_H
#define DBAPI_H

#include "file.hpp"
#include "bpt.hpp"
#include "buffer_manager.hpp"
#include "disk_manager.hpp"
#include "lock_manager.hpp"

int init_db(int buf_num);
int open_table(const char* pathname);
int db_insert(int tid, int64_t key, char* value);
int db_delete(int tid, int64_t key);
int db_find(int tid, int64_t key, char* ret_val, int trx_id);
int db_update(int table_id, int64_t key, char* values, int trx_id);
int close_table(int table_id);
int shutdown_db(void);

#endif // DBAPI_H
