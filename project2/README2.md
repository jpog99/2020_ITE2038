# Implementation of Disk Based B+ Tree
## 1. Layered Architecture (File Hierarchy)
In this project, we were given a task to provide a design of layered architecture. Each functions are divided separately into a few layers (ie. files) and calls upon each other from different layer. Some of the advantages of layered architecture are, the whole program code will not be cluttered in one single file and it will enable easier access for other developers. Furthermore, call flow will be optimized in situation where a function from another layer will not be called unless needed. This would prevent the call flow error and does not affect other unneeded functions while executing another function.

In this disk based B+ Tree, the whole program codes has been divided into 4 layers and the call flow are as below.
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project2/image/6.PNG)

## 2. File Manager API `file.c`
In this implementation, the program will provide several functions which are insert, delete and find from some external database file. Thus, a layer which will handle the I/O between the external file to our memory is needed. In this project, that handling service will be provided by File Manager APIs which are defined in 'file.c' and is set at the lowest layer, closest to the file layer. Thus, the functions from other layers will use these APIs to write, read, free page or assign page corresponding to the database from the external file.

This `file.c` provides the following file handling APIs:
### 1. `file_alloc_page()`
This file handles the assignment of newly created page `page_t` and return a page number `pagenum_t` to that page. The assignment is done by first reading the header page of the disk, get the `free_page_no` and returns it. In case of empty database file, it uses the function `memset()` from `<stdio.h>` and allocate the page by offset 0 (the first page in the file) and allocate it according to the specified page size (in this case `PAGE_FILE` = 4096). Before returning the new `pagenum_t`, the `free_page_no` in the header page will be updated for the next page assignment.

### 2. `file_free_page(pagenum_t)`
After deletion of a record (key/value pair), if the current page has `num_keys==0`, the page has to be deleted completely and set as a free page. Thus, if such case occurs, this function will be called upon the deletion process to free the page from the memory. This function receives the page number to be deleted `pagenum_t`, takes the parent page number and set is as `free_page_no` in header and the old `free_page_no` will be assigned to the `pagenum_t` from argument. Simply speaking, it's just assigning the page number of deleted page to the next free page number in header.

### 3. `file_read_page(pagenum_t , page_t*)`
This functions is used to receive a newly created page `page_t` and assign it to the corresponding `pagenum_t` from the argument. The page will receive the information from the disk, store it temporarily in memory and used by the upper layer to access the data. By using `pread(db, dest, PAGE_SIZE, pagenum*PAGE_SIZE)` from `<unistd.h>` , we will pass the `PAGE_SIZE` as argument of size and set the offset at pagenum*PAGE_SIZE. In other words, read file `db` from position `pagenum*PAGE_SIZE` and assign it to `dest` with allocation size of `PAGE_SIZE`.

### 4. `file_read_page(pagenum_t , page_t*)`
After editing the page from the function above, we will need to update the corresponding page into the disk and `pread(db, src, PAGE_SIZE, pagenum*PAGE_SIZE)` is used to complete the function. The arguments are similar except `src` argument indicates the modified page to be write.

### 5. `file_read_header()` and `file_write_header()`
This functions is very similar to the write/read functions from above. Except this time, this function is specially reserved for reading and writing the header page. Thus, the difference are `src` and `dest` are set to the `main_header` page and the offset is set to `0` as headers are the first page in the file.

## 2. Structure Remodelization 
In the initial code where it was implemented as in-memory, changes had to be done to the structure of the nodes to satisfy the project requirements such as default `LEAF_ORDER` from 4 to 32, `node*` type structure to `page_t` type and `int* keys` type to `record_t` type which provides both key and value.

The structure remodelization is defined in header file `file.h` as below.
### 1. Record(for leaf) and key/page pair (for internal)
```c
//record (leaf level)
typedef struct record_t {
    int64_t key;
    char value[VALUE_SIZE];
} record_t;

//key/page pair (internal node level)
typedef struct key_page_pair{
	int64_t key;
	pagenum_t pnum;
}key_page_pair;
```
### 2. Main header page
```c
typedef struct header_page_t{
	pagenum_t free_page_no;
	pagenum_t root_page_no;
	pagenum_t number_of_pages;
	char reserved[4072];
}header_page_t;
```

### 3. Free page 
```c
typedef struct free_page_t{
	pagenum_t next_free_page_pnum;
	char unused[4072];
}free_page_t;
```
### 4. Node (Leaf/Internal) page
```c
 //page header for leaf/internal
typedef struct page_header_t {
    pagenum_t parent_pnum;
    int is_leaf;
    int num_keys;
    char reserved[104];
} page_header_t;

//leaf/internal node
typedef struct node_t{
    page_header_t header;

    union{
        pagenum_t right_sibling_pnum;
        pagenum_t one_more_page_pnum;
    };

    union{
        record_t records[MAX_LEAF_ORDER-1];
        key_page_pair kp_pair[MAX_INTERNAL_ORDER-1];
    };
}node_t;
```
### 5. `page_t` type
```c
typedef struct page_t{
    //in-memory page structure
    union{
        header_page_t header_page;
        node_t node_page;
        free_page_t free_page;
    };
}page_t;
```
In this implementation, `page_t` will be used to create a new temporary page in memory to hold the data from file, and this struct is needed to differentiate whether that page will be header, free or node.

### 6. Global variable
```c
page_t main_header;
int db
```
We will provide 2 global variables which are `int db` for storing the `unique_id` of a file and `page_t main_header` as header page of a file. These 2 variables will be accessed from multiple layers. At first, the header was defined by using `header_page_t main_header` as global variable. But in later process, this has caused several segmentation error and my assumption is because both types holds different amount of size. Defining header as `page_t main_header` and accessing header metadata through `main_header.header_page` solved the problem. 

## 4. Code Transformation from In-Memory to On-Disk Implementation
In this project, we only need to provide 3 main operations which are insert, delete find. So, firstly we will ignore other operation functions such as `print_tree` and `find_range`.

### 1. Find Operation
From the initial implementation code of find operation which are mainly `find()` and `find_leaf()`, there are only a few changes made because find operation does not require any structure modifications at all and only require access to read the file data. So, from `main.c`, the `db_find()` is called and next calls the master find function `find()` and receive key and value as arguments. All it needs to do is read the file header page, locate the page number `pagenum_t` of the page that contains the key provided and prints the key and value pair in main function. If key is not found, returns `FAILURE` (=1), else return `SUCCESS` (=0);

### 2. Insertion Operation
Insertion operations are a bit harder than find because not only you need to insert a new pair of key/value, you also required to first gain access to the data file, uses `find()` operation to check duplicates, modify the page, and finally write the change to the disk. So, all you need first is change all initial functions that returns `node*` and change the return type to `pagenum_t` (In disk-based implementation, the location of records are navigated by `pagenum_t` type, likewise how in-memory that used `node*` for navigation). After finding the location to insert, create a new page and give it page number by using file_alloc_page(), copy the key/value that was passed as argument from `main.c` and then write changes to disk using `file_write_page()`. Luckily, the process of splitting has only few changes so we can use the algorithm of splitting as it is.

### 3. Deletion Operation
Unlike the first 2 operation above, deletion was a bit different to implement because on-disk implementation, our implementation would be optimized by including the semantics of delayed merge. If we uses the initial in-memory style of coalesce and redistribution, it would require the program to read and write onto the file multiple times before deletion operation is done. By delayed merge, the program will wait until all keys in a page are deleted and after that finally write changes to disk. In other words, we can say as *ignoring* the coalesce and redistribution process of a page and only write change to disk if that page is blank.   

## 5. Delayed Merge Implementation
Here we will look into the detailed process of how `redistribution` and `coalesce` functions works. Each operation are divided into 2 cases, whether the current node is at the far left or not.

### 1. `coalesce_nodes()` 
This operation is divided into 2 cases, either the current page is at leftmost or not. If it is, swap it with right neighbor and keep the neighbor child as current child. Else, copy all the child of the current page to the child at the left page. Finally delete the current page.
 
### 2. `redistribution_nodes()`  
If the current page is at the leftmost position, we will read the rightmost child of the right neighbor and copy it as current rightmost child. Else, the current page will have a left neighbor and copy the left neighbor's leftmost child.

These 2 functions works differently according if the current page is an internal page or leaf page. In case if internal page, the `key_page_pair` will be copied. Else if in case where current node is leaf, key/value `record_t` type will be modified according to the functions. 
