# Implementing Lock Table Module with POSIX Programming
By definition, POSIX threads is an execution model that allows a program to run and control multiple functions at a time. Which means, the program will execute 2 or more function at a single time frame in a parallel mannerism. The difference between the standard serial program execution, where program executes the program line by line and threaded execution can be visualized as below.   

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/1.PNG)

As shown in the image, all the functions in the program get executed together at the same time. The benefits of using threaded execution is that it will reduce the running time of the program greatly when handling with a very large amount of multiple procedures (eg. looping millions of times).

In this project, we will implement a lock table module using a given test code `unittest_lock_table`. In threaded program, a lock is necessary to ensure the atomicity of the function. This means, during the parallel execution, a thread must wait for another thread to finish using a function. After the current thread finished using the function, it will alert the next waiting thread so that the next thread will be able to use the function. In other words, we are **preventing from multiple thread using a same function at the same time**. `Mutex` is used as the lock in this kind of situation.

## 1. Analyzing Test Code [`unittest_lock_table`](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/unittest/unittest_lock_table.cpp)
From the [main](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/unittest/unittest_lock_table.cpp#L128) function, The program first initialize the 2d `account` array, holding `table_id` and `key` pair. Few arrays of threads for scanning and transfer function also initialized. First, [`init_lock_table`](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/unittest/unittest_lock_table.cpp#L143) function is called to initialize hash table and mutex `lock_table_latch`. Then, both thread arrays for transferring and scanning is initialized. 

During this instance, these multiple threads will execute their corresponding functions at the same time. Main utility for this program is transferring money value from one random <table_id,key> pair to another random pair by decrementing the value of former and incrementing the latter pair at random money value not more than 100 (refer [line 18](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/unittest/unittest_lock_table.cpp#L18) ). After 1,000,000 times of transferring and scanning, the threads then get joined to the main thread before terminating the program. If the procedure is successful, the sum of the total balances is unchanged. 

## 2. Initializing Struct for `lock` and `hash_table_entry`

### 1. Lock Struct Definition

```cpp
struct lock_t {
	/* NO PAIN, NO GAIN. */
	lock_t* prev; //points to the prev lock in lock list 
	lock_t* next; // points to the next lock in lock list
	unordered_map< pair<int,int> , pointer , hash_pair>::iterator sentinel_pointer; //points to the hash table entry for the corresponding <table_id,key> pair.
	pthread_cond_t cond; // conditional variable of the lock
};
``` 
As specified in the project specification, a lock list is initialized at every corresponding `<table_id,key>` pair. This lock list is initialized in a double linked list structure, thus the struct having pointers `next` and `prev`. Every lock has a conditional variable `cond` that is used for getting notified when other thread finishes using a function. `sentinel_pointer` helps the lock to identify which hash table entry lock list that this lock belongs to. 

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/2.PNG)

### 2. Hash Table Entry Definition. 

In this project, we will use [unordered_map](http://www.cplusplus.com/reference/unordered_map/) to implement our hash table. By using unordered_map we can uniquely identify the element (ie. table_id,key pair) and allow for fast access to individual elements directly by their key values. Our hash table entry also should have `head` to point to the head of lock list and `tail` to point to the tail of lock list. 

```cpp
typedef struct pointer{
	lock_t* head;
	lock_t* tail;
}pointer;
```

But, to achieve such implementation that uses unordered_map for easier element access, we want the `<table_id,key> pair` as the key value of map and `prev/head pointers` as the value of map. Recall that we want a lock list that correspond to the `<table_id,key> pair`. Unordered Map does not contain a hash function for a pair key. We need to explicitly provide the hash function that can hash a pair.
This hash function will provide us the hash value of given pair.

(*from [`lock_table.h` line 19](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/include/lock_table.h#L19)*)
```cpp
struct hash_pair { 
    template <class T1, class T2> 
    size_t operator()(const pair<T1, T2>& p) const
    { 
        auto hash1 = hash<T1>{}(p.first); 
        auto hash2 = hash<T2>{}(p.second); 
        return hash1 ^ hash2; 
    } 
}; 
```
*Source: [(GeeksForGeeks) How to create an unordered_map of pairs](https://www.geeksforgeeks.org/how-to-create-an-unordered_map-of-pairs-in-c/)*

Finally, we can initialize our hash table entry as below:
```cpp
unordered_map< pair<int,int> , pointer , hash_pair> hash_table_entry;
```
* pair<int,int> : First argument as key of map.
* pointer       : Second argument as value for corresponding key in map.
* hash_pair     : Third argument as the function used to hash the given key.

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/3.PNG)

## 3. Lock Table APIs [`lock_table.cpp`](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp)

### 1. `int init_lock_table(void)`

```cpp
int init_lock_table()
{
	lock_table_latch = PTHREAD_MUTEX_INITIALIZER;	
	return SUCCESS;
}
```
This function will initialize any structures necessary for implementing lock module such as hash table entry and lock table latch. We already initialized the hash table entry as a global variable (*refer [line 14](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L14)*) as it is used by all APIs.

* Why there is no error checking during initializing mutex?

Using PTHREAD_MUTEX_INITIALIZER, error checking is not necessary. This is because static initialization cannot fail. It is designed to treat a zero initialized `pthread_mutex_t` as if it had been initialized to all default values.

### 2. `lock_t* lock_acquire(int table_id, int64_t key)`

This function will create a new `*lock_t` object and appends it to the lock list corresponding to the `<table_id,key>` pair that is received from the function argument.

From [line 36](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L36), using one of the function from unordered_map container `.find()`, we find the pointer that points to the map key that matches the argument, we assign a variable `iter` which will store the hash table entry. Using auto, we can ignore the type specification for this variable and let the program itself assign it for us. 

Then, the function will append to the lock list based on a few different cases such as below.

* [Case 1: Hash table entry for <table_id,key> pair not found](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L39). 

```cpp
	if(iter == hash_table_entry.end()){
		hash_table_entry[p].head = lock;
		hash_table_entry[p].tail = lock->next;
		iter = hash_table_entry.find(p);
		lock->sentinel_pointer = iter;
	}
```
If the hash table entry is not available (due to map haven't store all hash table entry yet), then simply insert a new key value pair into the map. The "key" will be the argument `(table_id,key)`  pair that was passed into the function, while the "value" is `head` and `tail` pointers. `Head` is initialized to new lock, while `tail` initialized to `nullptr`. Then the `sentinel_pointer` of lock will point to the hash table entry `iter` itself. 

*Visualization:*   
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/4.PNG)

* [Case 2: Hash table entry found, but no lock list](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L57)

```cpp
		if(iter->second.head == nullptr){
			iter->second.head = lock;
			iter->second.tail = lock->next;
		}

```

Similar to previous case, but this time only adding lock into the lock list. From now on, the `sentinel_pointer` is already initialized locally under the case where hash table entry is found (*refer [line 48](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L48)*). Because this lock doesn't have any predecessor, we don't have to put this lock to sleep. 

*Visualization:*   
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/5.PNG)

* [Case 3: The lock list only have 1 lock, which is the head of list](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L57)

```cpp
			if(iter->second.head->next == nullptr){
				iter->second.tail = lock;
				iter->second.head->next = lock;
			}
```

This time we want to change the `tail` pointer to the newly created lock. So, `head->next` and `tail` will point to new lock.

*Visualization:* 

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/6.PNG)

* [Case 4: Hash table entry has multiple locks in lock list](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L63)

```cpp
			else{
				iter->second.tail->next = lock;
				iter->second.tail = lock;
			}
```

If the `head` and `tail` pointer of the hash table already pointing to lock, add the lock to the last position of the lock list. Then, `tail` and `tail->next` will point to the new lock.

*Visualization:* 

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/7.PNG)



For case 3 and 4, where the new lock have predecessor, we need to put the new lock to sleep and wait until the predecessor is released. 

```cpp
			while(lock != iter->second.head){
				pthread_cond_wait(&lock->cond, &lock_table_latch);
```

Finally, after the new lock is added, we [unlock the mutex](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L76) of this function so that the next thread can utilize it. Then, return the address of the newly created lock.

### 3. `int lock_release(lock_t* lock_obj)`

Same as previous function, we must [lock this function](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L84) so that only one thread can access this function at a time.

In this function, we want to remove the `lock_obj` that given in function argument from the lock list. To find which lock list we want to remove from, check the `sentinel_pointer` of the given `lock_obj` so that we have the hash table entry where the lock list is. 
Now that we know the lock list that contains `lock_obj`, we should consider several cases before deallocate the lock.

* [Case 1: FAILURE CASE](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L88)

```cpp
	if(lock_obj != iter->second.head)
		return FAILURE;
```

In this case, the program is trying to deallocate a lock that still have a predecessor. Based on the project specification, this is not allowed because **a lock can only be released if its predecessor is unlocked first**. The function will return non-zero value.

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/8.PNG)

* [Case 2: `lock_obj` has no successor](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L92)

```cpp
	if(lock_obj->next == nullptr){
		iter->second.head = nullptr;
		iter->second.tail = nullptr;
	}
```

This case, the `lock_obj` does not have any successor. So, we don't have to send wake up signal in this case. We just deallocate the lock like usual. `head` and `tail` pointer is nullified and the lock list is empty. 

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/9.PNG)


 * [Case 3: `lock_obj` has a successor](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L98)

```cpp
	if(lock_obj->next != nullptr){
		iter->second.head = lock_obj->next;
		iter->second.head->prev = nullptr;
		
		//wake up the next lock of lock_obj
		pthread_cond_signal(&lock_obj->next->cond);
	}
```

If the `lock_obj` has a successor, we change the `head` pointer to the successor, and now the successor's parent will become null pointer. After that, `lock_obj` will [send the signal](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/src/lock_table.cpp#L103) to wake up the successor.

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/10.PNG)


Finally, free the `lock_obj` , unlock the mutex, and return status code.

## 3. Running The Program and Analyze Result

After we implementing the APIs for out lock module, we can test if the module is successful or not by checking the output after running the test code. 

Based on the given test code [`unittest_lock_table.cpp`](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/unittest/unittest_lock_table.cpp), we can see that the program **8 threads** to run transfer function, and **1 thread** to run scan function (*refer [line 9](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/unittest/unittest_lock_table.cpp#L9) and [line 10](https://hconnect.hanyang.ac.kr/2020_ite2038_11800/2020_ite2038_2019007901/-/blob/master/project4/unittest/unittest_lock_table.cpp#L10)*). Every time `transfer_thread_func` is completed, `"Transfer thread is done.\n"` is printed. Every time `scan_thread_func` is completed, `"Scan thread is done.\n"` is printed.

* Expected result

`"Transfer thread is done.\n"` printed 8 times,
`"Scan thread is done.\n"` printed 1 time.

* Actual result

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/11.PNG)

Seems like our implementation is correct!

* Why is the running time so long?

After running this program several times, the running time is always around 20~25 seconds. This is caused by the loop that is defined in the test code. The test code is doing **1 million** loops for every `scan_thread_func` and `transfer_thread_func`. Even more, in every loop, we are doing API function calls for `lock_aquire` and `lock_release`.

* What if we reduce the loop count?

This time, we will reduce the loop count from 1,000,000 to only 100 in the test code. By reducing the loop count by ten thousand times, we can see that now our program is extremely fast. 

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project4/image/12.PNG) 
