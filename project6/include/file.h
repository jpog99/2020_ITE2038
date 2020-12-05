#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <pthread.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <list>

//page specifications
#define PAGE_SIZE 4096
#define RECORD_SIZE 128
#define VALUE_SIZE 120
#define MAX_LEAF_ORDER 32
#define MAX_INTERNAL_ORDER 249
#define MAX_FILENAME_LENGTH 20
#define MAX_TABLE_COUNT 10
#define BUFFER_SIZE 500
#define BEGIN 0
#define UPDATE 1
#define COMMIT 2
#define ROLLBACK 3
#define COMPENSATE 4
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
    int64_t page_LSN;
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
	std::string lru_next;
	std::string lru_prev;
	int in_use;
}frame_t;

typedef struct buffer_pool_t{
	std::unordered_map<std::string,frame_t> frames;
	int size;
	int used_frame_cnt;
	std::string mru_frame;
	std::string lru_frame;
}buffer_pool_t;

typedef struct table_id_list{
	int table_id;
	char pathname[MAX_FILENAME_LENGTH];
}table_id_list;


typedef struct lock_t lock_t;
typedef struct trx_t trx_t;

struct lock_t {
    int tid;
    int key;
    int lock_mode;
    trx_t* trx;
};

struct trx_t {
    int trx_id;
    std::string state;
    std::list<lock_t*> lock_list;
};

typedef struct log_t{
	int64_t LSN;
	int64_t prev_LSN;
	int trx_id;
	int log_type;
	int tid;
	int pnum;
	int offset;
	int data_len;
	char old_image[120];
	char new_image[120];
}log_t;



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
extern char filename[MAX_FILENAME_LENGTH];
extern int tid;
extern buffer_pool_t buf_pool;
extern int buf_exist;
extern table_id_list tid_list[MAX_TABLE_COUNT];
extern std::list<trx_t> trx_list;
//extern int trx_id;
extern std::unordered_map<std::string, lock_t*> lock_table;
extern volatile int64_t last_LSN;
extern std::vector<log_t*>log_buf;
extern volatile int flushed_idx;
extern volatile int log_fd;
extern FILE* logmsgFile;

extern pthread_mutex_t trx_mutex ;
extern pthread_mutex_t lock_mutex;
