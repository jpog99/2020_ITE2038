#ifndef DBAPI_H
#define DBAPI_H

#include "bpt.h"

int open_table(const char* pathname);
int db_insert(int64_t key, char* value);
int db_delete(int64_t key);
int db_find(int64_t key, char* ret_val);

#endif // DBAPI_H
