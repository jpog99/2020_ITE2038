# Concurrency Control in Database Management and Implementation

## 1. Definition 
Transaction (or TRX) is a user-defined instruction sequence, which holds the ACID properties as below
* Atomicity: Either the entire TRX runs successfully at once, or the TRX itself does not run.
* Consistency: Consistency of reading and writing values ​​is guaranteed.
* Isolation: The transaction is isolated with other transactions.
* Durability: The transaction must handle any error operations in any situation.

Isolation has several types, such as serializable, which means it is completely independent, and users can read uncommitted data to the extent that the user feels the system is independent from other users. The meaning of serializable transaction is that when multiple TRXs are executed at the same time and when they are processed in sequence, the same result is obtained. This can be satisfied by forcing the order of data access through lock.

The consistency of data in database can be threatened if several TRXs attempt to read and write the same data at the same time. There are mainly 3 major problem that can occur.
* Inconsistent read: When reading the value to be modified, the two values ​​read for one object may be different.
* Lost update: If two write operations occur at the same time, one write operation may be lost.
* Dirty read: When reading a temporary value that has not yet been confirmed, the value may be lost during the undo process.

## 2. Code Design and Implementation.
In this project, we need implement the concurrency control for our previous code so that our program can has transaction system that support both Consistency and Isolation. 

Different from previous project, this time I decided to change everything to C++ language because it would be easy to implement the lock tables and transaction tables using unordered_map<> and list<>. In this project, I initialized the `lock_table` using unordered_map<string,lock_t*> and both trx_list and lock_list using list<trx_t> and list<lock_t> respectively.

* `lock_t` and `trx_t`
```c
struct lock_t {
    int tid; //table_id of lock
    int key; //key of lock
    int lock_mode; //0=SHARED or 1=EXCLUSIVE
    trx_t* trx; //transaction for this lock
};

struct trx_t {
    int trx_id; //unique transaction id
    std::string state; //state of trx, either WORKING or IDLE
    std::list<lock_t*> lock_list; //lock list of the transaction
};
```   
* Global Variables
```c
extern std::list<trx_t> trx_list; //global transaction list
extern std::unordered_map<std::string, lock_t*> lock_table; // lock table that will store locks to corresponding tid.key pair

extern pthread_mutex_t trx_mutex ; //global transaction mutex
extern pthread_mutex_t lock_mutex; //lock mutex
```

Notice that this time we had to declare the global variables as extern. This is because when compiling the file as C++, there will be compiling error where these global variables are defined more than once. Extern keyword helped to solve that and the instead of initializing the global variables in the header, we initialize them in their respective source file.

* `table_id` and `frame_idx` pair

Worth to mention that in previous project, we describe the frame position in `buf_pool` using `int` type. But in this project, We have to always keep the frame index together with its respective `table_id` to insert into the `lock_table` variable. In project 4 we used the hash function to create an unordered map that contains pairs as map key. To reduce the hassle, in this project we only uses string to represent the string `table_id` and int `frame_idx` pair by the format "string.frame_idx".

## 3. `Lock_Manager.cpp`

* `int trx_begin(void)`

This function is called in `main` during the startup of the program to initialize the transaction structure. Here is where the `trx_mutex` are initialized and locked until the transaction is complete. We initialize a new `trx_t` here and add it into the `trx_list` variable.

* `int trx_commit(int trx_id)`

First, this function will iterate through the `trx_list` to find the TRX with corresponding `trx_id` from argument. If not found, then this function will return 0. Else, it will return the argument. After we fount the TRX, we unlock all the locks that added into the TRX `lock_list` one by one, committing all the process held by the lock. Finally, we erase the TRX from the global `trx_list` and unlocks its mutex. 
  
* `int init_lock_table(void)`

This function is pretty straightforward and simple. Similar to project 4, this function only initialize the lock mutex and returns 0 for successful initialization.

*  `lock_t* lock_acquire(int table_id, int64_t key, int trx_id, int lock_mode)`

This function is called when we want to acquire a lock onto a certain process. Firstly it will create a pointer `lock_t*` and initialize all its inner variables. Lock mode is given by the argument, where 0 means `SHARED` and 1 means `EXCLUSIVE`. After that, we lock the lock mutex so that this transaction (process running) is isolated from other transactions. Finally, we add the newly created lock into the global lock table and TRX's `lock_list`.

* `int lock_release(lock_t* lock_obj)`

To release a lock so that other waiting TRXs can enter the process, this function will find the transaction that contains the `lock_obj` from the argument in their `lock_list`. Then, we will delete the `lock_obj` from the list and unlock the lock mutex. 

## 4. `db_update()`, `db_find()` and Lock Mode

In this project, we will modify our old `find()` function so that it can support transaction implementation. Basically, the new find function is not so much different from the old one. In the new `find()`, we will pass the `trx_id` into the argument. Everytime this function is called, `lock_acquire()` will be called to acquire the lock so that the TRX corresponding to the `trx_id` is isolated while using this find function. The lock mode is set to SHARED (0) for find function. After the function passes the found value to the upper layer, the lock is unlocked and ready to be used by other TRXs.

`update()` function is very similar to `find()` function. In find function, we copy the value from the disk into our program variable. But in update function, it is the other way around. This time we copy the value passed from user input into the disk. This operation does not require any structure modification unlike insert and delete. `update()` function will use the EXCLUSUVE lock if a transaction acquires a lock in this function.

Thus, in `main.cpp`, we introduce a new command "u %d %ls %s" for update function.

## 5. Deadlock Detection and Rollback

For the deadlock detection, I attempted to list out the locks that points to the another waiting locks in the lock list and make sure if there is cycle (the last lock does not point to NULL) or not. Using the iterator, we iterate every locks in `lock_list` and check their subsequent locks. If there is a conflict (deadlock detected), put it as `wait_lock`, and use the lock's back pointer to find the TRX that has `wait_lock`. Then, abort the transactions that included in the deadlock, rollback all operations, and then release all locks.

But unfortunately , this was only an idea and to implement this was really challenging.
