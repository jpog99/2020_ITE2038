#ifndef BPT_H_INCLUDED
#define BPT_H_INCLUDED


// Uncomment the line below if you are compiling on Windows.
// #define WINDOWS
#include <string.h>
#include <stdint.h>
#include "file.h"
#ifdef WINDOWS
#define bool char
#define false 0
#define true 1
#endif


// Constants for printing part or all of the GPL license.
#define LICENSE_FILE "LICENSE.txt"
#define LICENSE_WARRANTEE 0
#define LICENSE_WARRANTEE_START 592
#define LICENSE_WARRANTEE_END 624
#define LICENSE_CONDITIONS 1
#define LICENSE_CONDITIONS_START 70
#define LICENSE_CONDITIONS_END 625



// FUNCTION PROTOTYPES.

// Output and utility.

//void license_notice( void );
//void print_license( int licence_part );
//void usage_1( void );
//void usage_2( void );
//void usage_3( void );
//print
//int height( node * root );
//int path_to_root( node * root, node * child );
//void print_leaves( void );
//void print_tree( void );
//void find_and_print(node * root, int key, bool verbose);

// Finding
pagenum_t find_leaf(pagenum_t root, int64_t key);
int find( int64_t key, char* value);
int cut( int length );

// Insertion.

record_t* make_record(int64_t key, char* value);
pagenum_t make_node( page_t* node );
pagenum_t make_leaf( page_t* leaf );
int get_left_index(page_t* parent, pagenum_t left_pnum);
int insert_into_leaf(pagenum_t leaf_pnum, page_t* leaf, int64_t key, char* value);
pagenum_t insert_into_leaf_after_splitting(pagenum_t root_pnum, pagenum_t leaf_pnum, page_t* leaf, int64_t key, char* value);
int insert_into_node(pagenum_t root_pnum, page_t* parent,
        int left_index, int64_t key, pagenum_t right_pnum);
pagenum_t insert_into_node_after_splitting(pagenum_t root_pnum, pagenum_t old_pnum,
                                        int left_index, int64_t key, pagenum_t right_pnum,page_t* old);
pagenum_t insert_into_parent(pagenum_t root_pnum, pagenum_t left_pnum, page_t* left, int64_t key, pagenum_t right_pnum, page_t* right);
pagenum_t insert_into_new_root(pagenum_t left_pnum, page_t* left, int64_t key, pagenum_t right_pnum, page_t* right);
pagenum_t start_new_tree(int64_t key, char* value);
int insert(int64_t key, char* value);

// Deletion.

int get_neighbor_index( page_t* parent, pagenum_t current_pnum );
int remove_entry_from_node(int64_t key, page_t* node, int type);
pagenum_t adjust_root(pagenum_t old_root_pnum);
pagenum_t coalesce_nodes(pagenum_t root_pnum, pagenum_t current_pnum, page_t* current,
                         pagenum_t neighbor_pnum, page_t* neighbor, int neighbor_index,
                        int64_t k_prime);
int redistribute_nodes(pagenum_t root_pnum, pagenum_t current_pnum, page_t* current, pagenum_t neighbor_pnum, page_t* neighbor, int neighbor_index, int64_t k_prime, int k_prime_index);
pagenum_t delete_entry( pagenum_t root_pnum, pagenum_t current_pnum, int64_t key );
int delete(int64_t key);

//void destroy_tree_nodes(node * root);
//node * destroy_tree(node * root);

#endif // BPT_H_INCLUDED

