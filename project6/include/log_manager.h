#pragma once

#include "dbapi.h"

//LOG MANAGER APIs

//create new log corresponding to trx_id
int64_t create_log(int trx_id , int log_type);
int64_t create_log(int trx_id , int log_type, int tid, int pnum, int idx, char* old_image, char* new_image);

//write log after commiting a transaction
void flush_log(void);

//recover the previous state after crash
int recover(int flag, int log_num, const char* log_path, const char* log_msgpath);

//write log to log file
void print_log();

