# Implementation of In-Memory Buffer Manager
In our previous implementation of B+ Tree, after every operation that was made by the user, the changes are saved straight into the disk. So, in this project, a new layer called buffer layer is introduced to manage the I/O operations on the disk. 

## 1. Buffer Pool
A buffer layer contains a buffer pool and this buffer pool contains several frames that hold the pages from the disk. So, instead of the upper layer requesting a page directly from the disk, the upper layer will check if the requested page exist in buffer pool. If not, then the buffer pool will fetch the requested page from disk and keep that page inside the buffer pool for next use.

All operations in our project (insert, delete, find) will read page from buffer pool only, unlike previous implementation where it finds the page directly from disk. This means, the previous file manager APIs are only called within the buffer layer. 
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project3/image/1.PNG)

If there was any modifications of the page occurred in the buffer pool, the buffer layer will wait before saving any changes to disk until that page is evicted from the buffer pool.  The buffer pool also can hold pages from different disk at a time. This will enable the users to execute any operations on multiple disk on a single run of the program. Thus, in this project we also implemented a new `table_id` system to handle the I/O operations on multiple disks. When the program is closed, all changes in buffer pool is saved to the respective disks and the buffer pool is destroyed.  

## 2. Buffer Manager Structure
1.  Frames in buffer pool
```c
typedef struct frame_t{
	page_t page;   //page that is held by this frame
	int table_id;  //table_id of the page
	pagenum_t pnum; //page number
	int is_dirty;  //shows if the page has been modified or not
	int is_pinned;  //shows if the page is currently in use by program
	int lru_next;  //the frame index of next frame
	int lru_prev;  //the frame index of previous frame
	int in_use;  //shows if this frame already have page or not
}frame_t;
```
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project3/image/2.PNG)

2. Buffer pool
```c
typedef struct buffer_pool_t{
	frame_t* frames; //the frames in pool
	int size; //total frames in pool
	int used_frame_cnt; //total of occupied frame
	int mru_frame; //index of most recently used frame
	int lru_frame; //index of least recently used frame
}buffer_pool_t;
```
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project3/image/3.PNG)

3. Globals

Because buffer pool is used everwhere in our program, we will declare a global variable for buffer pool `buf_pool`. The function `init_db(buffer_size)` will determine how many frames are allocated in the `buf_pool`.

## 3. Buffer Manager APIs `buffer_manager.h`
After implementing the structure of frame `frame_t` and buffer pool , we need a few additional functions that will manage how our buffer pool behaves. These APIs are mostly used by the upper layer for fetching page, reading buffer header and for the buffer pool to evict a page to add the next page.

1.  `buf_add_page()`

This function is called when we want to add a page from disk to `buf_pool`. There are 2 cases that are handled in this function. The first case is when there is free frame exist in the `buf_pool`, then find the free frame index, initialize all the variables in `frame_t` and return the frame index. For the second case where there is no free frame in `buf_pool`, use the clock LRU policy to find the `eviction victim` and get its frame index. If the page that was held by eviction victim is modified, write all changes to disk first and remove the page from the frame using `buf_remove_page()` to set it vacant for the page to be added. 

2. `buf_remove_page(int frame_idx)`

This function is called to set a frame in `buf_pool` vacant and flush the page onto disk. After the eviction victim `frame_idx` is selected, write all changes to disk if the variable `is_dirty` of `frame_idx` is 1. There are 3 cases to be handled. The first case is when `frame_idx` is LRU, then set the next frame as the next LRU frame. In the second case where `frame_idx` is current MRU frame in `buf_pool`, then set it's previous frame as the new MRU frame. In the final case, where `frame_idx` either has next or prev frame (middle of buffer pool), then just set the `frame_idx` prev or next accordingly. 

3. `buf_read_page(int table_id, pagenum_t pnum, int is_dirty)`

This function only get called when the upper layer is requesting to read a page in buffer pool and returns the contents of frame. After the page is read by the program, this function need to `pin` the returned frame to the `buf_pool`, indicating that this current frame is in use by the upper layer. In case where the requested page does not exist in `buf_pool`, this function will call `buf_add_page()` to add the page into the buffer pool. 

4. `adjust_mru_frame(int frame_idx)`

Everytime after a frame is being used by the program, we need to reorganize the buffer pool and mark which frame is the most recently used frame so that it will be less likely to become the eviction victim.  

5. `buf_get_tid(const char* pathname)`

This function reads the header of the disk into buffer pool and returns the `table_id` after opens it. This function also used to add the header of the disk to the buffer before starting any operation. The `table_id` will be used to reference which disk will be accessed for the future operations. 

## 4. Newly Implemented APIs from `dbapi.h`
 
* `init_db(int buffer_size)`

This function is used to determine the number of frames to be allocated in buffer_pool.

* `close_table(int table_id)`

Flush all frames that are corresponding to the `table_id` in argument and write the changes to disk. Then, update the table_id_list array.

* `int shutdown_db(void)`

This function acts very similar to `close_table()` function, but instead flush everything that contained in the buffer pool and write changes to their `table_id` respectively. Finally, deallocate all frames in buffer pool from memory.

## 5. New Operations in `main.c` for `table_id` System
In this project, we are expected to work with multiple disks and this can be achieved by giving each opened disk a distinct `table_id` to differentiate each table to enable the user to handle insertion, deletion and find operations on multiple disks at a time. All the `table_id and filenames are stored in `tid_list` that was initialized globally in `file.h`. Thus, a new operations has been added.

* `o <filename>` : Opens a file given by `filename`. If file not exist, create a new file with that filename. Filename must **not more than 20 characters long**.

* `t` : Display all the contents of `tid_list`

* `c <table_id>`: Closes the table correspond to `table_id` and saves all changes to that table.

All previous operations which are insert, find and delete now receive an additional argument `table_id` to let the user choose which table do themselves want to operate on. Operation quit `q` will call `shutdown_db()` to save all changes made and destroy the buffer poll in memory.

## 6. Major Modifications in Code 
1. In `bpt.c`, all previous `page_t` has changed to `frame_t` because now all the functions read the content of the page through the frame. 

2. In `bpt.c`, everytime before encountering return instruction in every function, instead of writing change to disk, only unpin the frame because all changes written only during page removal from buffer pool, closing table or shutdown.

3. In previous implementation, after every operation, update header in disk right away, but instead now only update the header in buffer pool first. 

4. In `file.h`, `file_alloc_page` and `file_free_page` became `buf_alloc_page` and `buf_free_page` respectively. This means the allocation and deallocation of pages is made inside the buffer pool instead of in disk. 


