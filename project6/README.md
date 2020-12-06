# Logging and ARIES Recovery

## 1. ARIES and Logging

Whenever a database system crash or encountered an unexpected problem, that database must perform a recovery where it reverts into its original state before the crash. In other words, the database ignores all the changes done after the crash and redo all the transaction that has been done from a specified checkpoint to the point before crash. ARIES recovery uses a system called **log sequence number (LSN)** to keep track all the operations that has been done by the users and to identify what updates that has been applied to the database page. So, each page in database will have a **PageLSN** which indicates LSN of the log records in which the effects are reflected on that page. This PageLSN is also required during redo pass to prevent any unnecessary repeated redo. 

In this project, we are required to implement ARIES logging algorithm which involves 3 passes. Analysis pass will determine which transaction operation to redo and which pages are dirty at the time of crash. After that, Redo pass will redo all the transaction operations from the RedoLSN point until the point before the system crashes. Finally, Undo pass will rollback all the incomplete transactions which their abort operation was not completed. 

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project6/img/1.PNG)

 ## 2. Structure Implementation

* Log structure

```c
typedef struct log_t{
	int64_t LSN; //the LSN number
	int64_t prev_LSN; //previous LSN number
	int trx_id; //Transaction ID corresponding to log
	int log_type; //types of operation which reflects the log activity
	int tid; //Table id of corresponding log
	int pnum; //page number where the log operation was done
	int offset; //Start offset of the modified area within a page.
	int data_len; //length of changed data
	char old_image[120]; //old data before operation
	char new_image[120]; //new data after operation
}log_t;
```

* Page header

```c
typedef struct page_header_t {
    //....
    int64_t page_LSN; //LSN of record which reflect the operation done onto this page
} page_header_t;
```

* Global variables

```c
extern volatile int64_t last_LSN; //indicates the last LLSN in log file
extern std::vector<log_t*>log_buf; //a vector which stores the logging data
extern volatile int flushed_idx; //the index of log which is recovered during recovery process
extern volatile int log_fd; //File descriptor for logfile.data file
extern FILE* logmsgFile; //.txt file where the contents of logs are written
```

## 3. Log Manager APIs

### 1. `create_log(int trx_id , int log_type, int tid, int pnum, int idx, char* old_image, char* new_image)`

This function is called every time an operation is started by the user. The function will initialize a `log_t` and all its variables accordingly to the parameter given. `Last_LSN` variable will be updated to be equal to this log LSN and will keep incremented when the next log is created. Finally, the log will be added into the global variable `log_buf`.

### 2. `void flush_log(void);`

This function is to flush all the logs in the `log_buf` into the specified "logfile.data" file where all the logs content will be saved. When a dirty page is about to get flushed into the disk, the program will first acquire an exclusive lock onto the page and then flush the page to the disk. After that, this function will be called to indicates that the logging of the changes made onto that page has been successfully saved into the log file. Finally, the lock on that page is released. 

### 3. `int recover(int flag, int log_num, const char* log_path, const char* log_msgpath);`

This function is where the three-pass algorithm of ARIES recovery is done.

* Analysis Pass

First, this function will open up the log file and iterate through all the logs. The program will keep track of last log record of each transaction. 

* Redo Pass

After that, it will handle 4 different cases according to the type of log. THe program will repeats the log operations by replaying every action not already reflected in the page on disk as follows. If the type is `BEGIN`, simple add the `trx_id` of that log into the `unordered_set loser` where it keeps the transaction id of which the transaction has started but have not committed yet. After that, create a new log. In `COMMIT` type case, the corresponding `trx_id` is deleted from the `loser`. In case of `ROLLBACK`, a new log is created so that it will start to store a new log from the last LSN. For `UPDATE` case, check the log page number `pnum` and fetch the page from disk. If the PageLSN of the page fetched from disk is less than the LSN of the log record, redo the log record. 

* Undo Pass

Finally, for the Undo pass, the program will perform a backward scan on log from the log buffer and undo all transaction in undo-list. In other words, iterate the log buffer backwards. If any of the log has `trx_id` that exist in the `loser` and the log type is `UPDATE`, then fetch the page from disk and undo the changes made on that page. 

This concludes the three-pass algorithm of the ARIES recovery.

### 4. `void print_log();`

This function is will write all the contents in log buffer into an external readable text file.

## 4. Changes From Previous Project

### 1. `int init_db(int buf_num, int flag, int log_num, const char* log_path, const char* logmsg_path)`

The function for database initialization will receive more parameters and have several added roles. The recovery function `recover()` will be called by this function as specified in the project specification. `flag` is used to determine the recovery protocol, `log_num` is needed for redo/undo crash, `log_path` is the file name for the log file (initialized as constant "logfile.data" string) and finally `logmsg_path` is the file name for text file that contains the strings of logs (initialized as constant "logmsg.txt" string).

### 2. `int trx_abort(int trx_id)`

A new log is created with log type `ROLLBACK` and all the logs in the log buffer are iterated backwards. Then the program start the rollback process. Logs with type `UPDATE` will fetch the page from the disk and changes the content of that page to the content before modification (`old_image`) to the page. If a log type `BEGIN` is encountered, exit the iteration. Finally, commit the changes by calling `trx_commit()` function. 

## 5. Test Result

* DB files, *logfile.data* and *logmsg.txt* are created after database initialization.

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project6/img/2.PNG)

* Contents of *logmsg.txt* after initialization and an operation

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project6/img/3.PNG)
