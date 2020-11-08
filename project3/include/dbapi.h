#ifndef DBAPI_H
#define DBAPI_H

#include "bpt.h"

int init_db(int buf_num);
int open_table(const char* pathname);
int db_insert(int tid, int64_t key, char* value);
int db_delete(int tid, int64_t key);
int db_find(int tid, int64_t key, char* ret_val);
int close_table(int table_id);
int shutdown_db(void);

#endif // DBAPI_H
