#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>


//page specifications
#define PAGE_SIZE 4096
#define RECORD_SIZE 128
#define VALUE_SIZE 120
#define MAX_LEAF_ORDER 32
#define MAX_INTERNAL_ORDER 249
#define MAX_FILENAME_LENGTH 20
#define MAX_TABLE_COUNT 10
#define BUFFER_SIZE 20
typedef uint64_t pagenum_t;

//return value const
#define SUCCESS 0
#define FAILURE 1

#define LEAF 1
#define INTERNAL 0

// TYPES.

/* Type representing the record
 * to which a given key refers.
 * In a real B+ tree system, the
 * record would hold data (in a database)
 * or a file (in an operating system)
 * or some other information.
 * Users can rewrite this part of the code
 * to change the type and content
 * of the value field.
 */

//record
typedef struct record_t {
    int64_t key;
    char value[VALUE_SIZE];
} record_t;

//key/page pair
typedef struct key_page_pair{
	int64_t key;
	pagenum_t pnum;
}key_page_pair;

/* Type representing a node in the B+ tree.
 * This type is general enough to serve for both
 * the leaf and the internal node.
 * The heart of the node is the array
 * of keys and the array of corresponding
 * pointers.  The relation between keys
 * and pointers differs between leaves and
 * internal nodes.  In a leaf, the index
 * of each key equals the index of its corresponding
 * pointer, with a maximum of order - 1 key-pointer
 * pairs.  The last pointer points to the
 * leaf to the right (or NULL in the case
 * of the rightmost leaf).
 * In an internal node, the first pointer
 * refers to lower nodes with keys less than
 * the smallest key in the keys array.  Then,
 * with indices i starting at 0, the pointer
 * at i + 1 points to the subtree with keys
 * greater than or equal to the key in this
 * node at index i.
 * The num_keys field is used to keep
 * track of the number of valid keys.
 * In an internal node, the number of valid
 * pointers is always num_keys + 1.
 * In a leaf, the number of valid pointers
 * to data is always num_keys.  The
 * last leaf pointer points to the next leaf.
 */
 //header page
typedef struct header_page_t{
	pagenum_t free_page_no;
	pagenum_t root_page_no;
	pagenum_t number_of_pages;
	char reserved[4072];
}header_page_t;

//free page
typedef struct free_page_t{
	pagenum_t next_free_page_pnum;
	char unused[4072];
}free_page_t;

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

typedef struct page_t{
    //in-memory page structure
    union{
        header_page_t header_page;
        node_t node_page;
        free_page_t free_page;
    };
}page_t;

typedef struct frame_t{
	page_t page;
	int table_id;
	pagenum_t pnum;
	int is_dirty;
	int is_pinned;
	int lru_next;
	int lru_prev;
	int in_use;
}frame_t;

typedef struct buffer_pool_t{
	frame_t* frames;
	int size;
	int used_frame_cnt;
	int mru_frame;
	int lru_frame;
}buffer_pool_t;

typedef struct table_id_list{
	int table_id;
	char pathname[MAX_FILENAME_LENGTH];
}table_id_list;

// GLOBALS.

/* The order determines the maximum and minimum
 * number of entries (keys and pointers) in any
 * node.  Every node has at most order - 1 keys and
 * at least (roughly speaking) half that number.
 * Every leaf has as many pointers to data as keys,
 * and every internal node has one more pointer
 * to a subtree than the number of keys.
 * This global variable is initialized to the
 * default value.
 */
//page_t main_header;
char filename[MAX_FILENAME_LENGTH];
int tid;
buffer_pool_t buf_pool;
int buf_exist;
table_id_list tid_list[MAX_TABLE_COUNT];


//FILE MANAGER APIs

// Allocate an on-disk page from the free page list
frame_t* buf_alloc_page(int tid, frame_t* buf_header);
// Free an on-disk page to the free page list
void buf_free_page(int tid, frame_t* buf_header, frame_t* page, pagenum_t pagenum);
// Read an on-disk page into the in-memory page structure(dest)
void file_read_page(int tid, pagenum_t pagenum, frame_t* dest);
// Write an in-memory page(src) to the on-disk page
void file_write_page(int tid, pagenum_t pagenum, const frame_t* src);
//Read header of db file
frame_t* buf_read_header(int tid, int is_dirty);

