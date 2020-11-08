/*
 *  bpt.c
 */
#define Version "1.14"
/*
 *
 *  bpt:  B+ Tree Implementation
 *  Copyright (C) 2010-2016  Amittai Aviram  http://www.amittai.com
 *  All rights reserved.
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.

 *  3. Neither the name of the copyright holder nor the names of its
 *  contributors may be used to endorse or promote products derived from this
 *  software without specific prior written permission.

 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.

 *  Author:  Amittai Aviram
 *    http://www.amittai.com
 *    amittai.aviram@gmail.edu or afa13@columbia.edu
 *  Original Date:  26 June 2010
 *  Last modified: 17 June 2016
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any node.
 *
 */

#include "bpt.h"
#include "buffer_manager.h"
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


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
//int order = DEFAULT_ORDER;

/* The queue is used to print the tree in
 * level order, starting from the root
 * printing each entire rank on a separate
 * line, finishing with the leaves.
 */
//node * queue = NULL;

/* The user can toggle on and off the "verbose"
 * property, which causes the pointer addresses
 * to be printed out in hexadecimal notation
 * next to their corresponding keys.
 */
//bool verbose_output = false;


// FUNCTION DEFINITIONS.

// OUTPUT AND UTILITIES

/* Copyright and license notice to user at startup.
 */
//void license_notice( void ) {
//    printf("bpt version %s -- Copyright (C) 2010  Amittai Aviram "
//            "http://www.amittai.com\n", Version);
//    printf("This program comes with ABSOLUTELY NO WARRANTY; for details "
//            "type `show w'.\n"
 //           "This is free software, and you are welcome to redistribute it\n"
 //           "under certain conditions; type `show c' for details.\n\n");
//}


/* Routine to print portion of GPL license to stdout.
 */
//void print_license( int license_part ) {
//    int start, end, line;
//    FILE * fp;
//    char buffer[0x100];
//
//    switch(license_part) {
//    case LICENSE_WARRANTEE:
//        start = LICENSE_WARRANTEE_START;
//        end = LICENSE_WARRANTEE_END;
//        break;
//    case LICENSE_CONDITIONS:
//        start = LICENSE_CONDITIONS_START;
//        end = LICENSE_CONDITIONS_END;
//        break;
//    default:
//        return;
//    }
//
//    fp = fopen(LICENSE_FILE, "r");
//    if (fp == NULL) {
//        perror("print_license: fopen");
//        exit(EXIT_FAILURE);
//    }
//    for (line = 0; line < start; line++)
//        fgets(buffer, sizeof(buffer), fp);
//    for ( ; line < end; line++) {
//        fgets(buffer, sizeof(buffer), fp);
//        printf("%s", buffer);
//    }
//    fclose(fp);
//}


/* First message to the user.
 */
//void usage_1( void ) {
//    printf("B+ Tree of Order %d.\n", order);
//    printf("Following Silberschatz, Korth, Sidarshan, Database Concepts, "
//           "5th ed.\n\n"
//           "To build a B+ tree of a different order, start again and enter "
//           "the order\n"
//           "as an integer argument:  bpt <order>  ");
//    printf("(%d <= order <= %d).\n", MIN_ORDER, MAX_ORDER);
//    printf("To start with input from a file of newline-delimited integers, \n"
//           "start again and enter the order followed by the filename:\n"
//           "bpt <order> <inputfile> .\n");
//}
//
//
///* Second message to the user.
// */
//void usage_2( void ) {
//    printf("Enter any of the following commands after the prompt > :\n"
//    "\ti <k>  -- Insert <k> (an integer) as both key and value).\n"
//    "\tf <k>  -- Find the value under key <k>.\n"
//    "\tp <k> -- Print the path from the root to key k and its associated "
//           "value.\n"
//    "\tr <k1> <k2> -- Print the keys and values found in the range "
//            "[<k1>, <k2>\n"
//    "\td <k>  -- Delete key <k> and its associated value.\n"
//    "\tx -- Destroy the whole tree.  Start again with an empty tree of the "
//           "same order.\n"
//    "\tt -- Print the B+ tree.\n"
//    "\tl -- Print the keys of the leaves (bottom row of the tree).\n"
//    "\tv -- Toggle output of pointer addresses (\"verbose\") in tree and "
//           "leaves.\n"
//    "\tq -- Quit. (Or use Ctl-D.)\n"
//    "\t? -- Print this help message.\n");
//}
//
//
///* Brief usage note.
// */
//void usage_3( void ) {
//    printf("Usage: ./bpt [<order>]\n");
//    printf("\twhere %d <= order <= %d .\n", MIN_ORDER, MAX_ORDER);
//}



///* Traces the path from the root to a leaf, searching
// * by key.  Displays information about the path
// * if the verbose flag is set.
// * Returns the leaf containing the given key.
// */
pagenum_t find_leaf(int tid, pagenum_t root, int64_t key){
    int i = 0;
    pagenum_t pnum = root;
    frame_t* p = buf_read_page(tid, pnum, 0);
    if (pnum == 0) {
        printf("Empty tree.\n");
        return pnum;
    }
    while (!p->page.node_page.header.is_leaf) {
        i = 0;
        while (i < p->page.node_page.header.num_keys) {
            if (key >= p->page.node_page.kp_pair[i].key) i++;
            else break;
        }
        if (i==0)
            pnum = p->page.node_page.one_more_page_pnum;
        else
            pnum = p->page.node_page.kp_pair[i-1].pnum;
        p->is_pinned = 0;
        p = buf_read_page(tid, pnum, 0);
    }
    p->is_pinned = 0;
    return pnum;
}


///* Finds and returns the record to which
// * a key refers.
// */
int find(int tid, int64_t key, char* value) {
    //add header in buffer pool
    frame_t* buf_header;
    buf_header = buf_read_header(tid, 0); 
    if(buf_header->page.header_page.root_page_no == 0){
         buf_header->is_pinned = 0;
         printf("Empty tree!\n");
         return FAILURE;
    }
    
    int i;
    pagenum_t c = find_leaf(tid, buf_header->page.header_page.root_page_no , key);
    
    if (c == 0){
     printf("Key %ld not found!\n",key);
     return FAILURE;
     }
     
    frame_t* leaf = buf_read_page(tid, c, 0);
    for (i = 0; i < leaf->page.node_page.header.num_keys; i++){
        if (leaf->page.node_page.records[i].key == key) break;
    }
    if (i == leaf->page.node_page.header.num_keys){
       leaf->is_pinned = 0; 
       return FAILURE;
       }
    else
        strcpy(value,leaf->page.node_page.records[i].value);
        leaf->is_pinned = 0;
        return SUCCESS;
}

///* Finds the appropriate place to
// * split a node that is too big into two.
// */
int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}


// INSERTION
//
///* Creates a new record to hold the value
// * to which a key refers.
// */
record_t* make_record(int64_t key, char* value) {
    record_t* new_record;
    if (new_record == NULL) {
        perror("Record creation.");
	return NULL;
    }
    else {
        new_record->key = key;
        strcpy(new_record->value , value);
    }
    return new_record;
}


///* Creates a new general node, which can be adapted
// * to serve as either a leaf or an internal node.
// */
frame_t* make_node(int tid, frame_t* buf_header ) {
    frame_t* p = buf_alloc_page(tid, buf_header);
    memset(p,0,PAGE_SIZE);
    return p;
}

///* Creates a new leaf by creating a node
// * and then adapting it appropriately.
// */
frame_t* make_leaf(int tid, frame_t* buf_header ) {
    frame_t* leaf = make_node(tid, buf_header);
    leaf->page.node_page.header.is_leaf = 1;
    return leaf;
}


///* Helper function used in insert_into_parent
// * to find the index of the parent's pointer to
// * the node to the left of the key to be inserted.
// */
int get_left_index(frame_t* parent, pagenum_t left_pnum) {
    if(parent->page.node_page.one_more_page_pnum == left_pnum) return 0;
    int left_index = 1;
    while (left_index <= parent->page.node_page.header.num_keys &&
            parent->page.node_page.kp_pair[left_index-1].pnum != left_pnum)
        left_index++;
    return left_index;
}

///* Inserts a new pointer to a record and its corresponding
// * key into a leaf.
// */
int insert_into_leaf(int tid, pagenum_t leaf_pnum, frame_t* leaf, int64_t key, char* value ){
    int insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->page.node_page.header.num_keys && leaf->page. node_page.records[insertion_point].key < key)
        insertion_point++;

    for (int i = leaf->page.node_page.header.num_keys; i > insertion_point; i--) {
        leaf->page.node_page.records[i].key = leaf->page.node_page.records[i-1].key;
        strcpy(leaf->page.node_page.records[i].value, leaf->page.node_page.records[i-1].value);
    }
    leaf->page.node_page.records[insertion_point].key = key;
    strcpy(leaf->page.node_page.records[insertion_point].value, value);
    leaf->page.node_page.header.num_keys++;
    //file_write_page(leaf_pnum, leaf);
    return SUCCESS;
}
//
//
///* Inserts a new key and pointer
// * to a new record into a leaf so as to exceed
// * the tree's order, causing the leaf to be split
// * in half.
// */
pagenum_t insert_into_leaf_after_splitting(int tid, frame_t* buf_header, pagenum_t root_pnum, pagenum_t leaf_pnum, frame_t* leaf, int64_t key, char* value) {

    frame_t* new_leaf = make_leaf(tid, buf_header);
    pagenum_t new_leaf_pnum = new_leaf->pnum;
    int temp_keys[MAX_LEAF_ORDER];
    char temp_val[MAX_LEAF_ORDER][VALUE_SIZE];
    int insertion_index, split, new_key, i, j;


    insertion_index = 0;
    while (insertion_index < MAX_LEAF_ORDER - 1 && leaf->page.node_page.records[insertion_index].key < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->page.node_page.header.num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf->page.node_page.records[i].key;
        strcpy(temp_val[j], leaf->page.node_page.records[i].value);
    }
    temp_keys[insertion_index] = key;
    strcpy(temp_val[insertion_index] , value);

    split = cut(MAX_LEAF_ORDER - 1);
    
    for(i = 0; i < split; i++) {
        leaf->page.node_page.records[i].key = temp_keys[i];
        strcpy(leaf->page.node_page.records[i].value, temp_val[i]);
        
    }
    leaf->page.node_page.header.num_keys = split;

    for (i = split, j = 0; i < MAX_LEAF_ORDER; i++, j++) {
        new_leaf->page.node_page.records[j].key = temp_keys[i];
        strcpy(new_leaf->page.node_page.records[j].value, temp_val[i]);
        
    }
    new_leaf->page.node_page.header.num_keys = MAX_LEAF_ORDER - split;

    new_leaf->page.node_page.right_sibling_pnum = leaf->page.node_page.right_sibling_pnum;
    leaf->page.node_page.right_sibling_pnum = new_leaf_pnum;
    new_leaf->page.node_page.header.parent_pnum = leaf->page.node_page.header.parent_pnum;
    new_key = new_leaf->page.node_page.records[0].key;
    

    //file_write_page(leaf_pnum, leaf);
    //file_write_page(new_leaf_pnum, &new_leaf);

    pagenum_t new_parent_pnum = insert_into_parent(tid, buf_header, root_pnum, leaf_pnum, leaf, new_key, new_leaf->pnum, new_leaf);
    new_leaf->is_pinned = 0;
    return new_parent_pnum;
}

///* Inserts a new key and pointer to a node
// * into a node into which these can fit
// * without violating the B+ tree properties.
// */
int insert_into_node(int tid, pagenum_t root_pnum, frame_t* n, int left_index, int64_t key, pagenum_t right_pnum){
    int i;
    
    for (i = n->page.node_page.header.num_keys; i > left_index; i--) {
        n->page.node_page.kp_pair[i].key = n->page.node_page.kp_pair[i-1].key;
        n->page.node_page.kp_pair[i].pnum = n->page.node_page.kp_pair[i-1].pnum;
    }
    n->page.node_page.kp_pair[left_index].key = key;
    n->page.node_page.kp_pair[left_index].pnum = right_pnum;
    n->page.node_page.header.num_keys++;
    return SUCCESS;
}
//
//
///* Inserts a new key and pointer to a node
// * into a node, causing the node's size to exceed
// * the order, and causing the node to split into two.
// */
pagenum_t insert_into_node_after_splitting(int tid, frame_t* buf_header, pagenum_t root_pnum, pagenum_t old_pnum, int left_index, int64_t key, pagenum_t right_pnum,frame_t* old){

    int i, j;
    int64_t k_prime;
    frame_t* new = make_node(tid, buf_header), *child;
    pagenum_t new_pnum = new->pnum;
    int64_t temp_keys[MAX_INTERNAL_ORDER];
    pagenum_t temp_pnum[MAX_INTERNAL_ORDER+1];


    /* First create a temporary set of keys and pointers
     * to hold everything in order, including
     * the new key and pointer, inserted in their
     * correct places.
     * Then create a new node and copy half of the
     * keys and pointers to the old node and
     * the other half to the new.
     */

   // temp_pointers = malloc( (order + 1) * sizeof(node *) );
   // if (temp_pointers == NULL) {
   //     perror("Temporary pointers array for splitting nodes.");
   //     exit(EXIT_FAILURE);
   // }
   // temp_keys = malloc( order * sizeof(int) );
   // if (temp_keys == NULL) {
   //     perror("Temporary keys array for splitting nodes.");
   //     exit(EXIT_FAILURE);
   // }
    temp_pnum[0] = old->page.node_page.one_more_page_pnum; 
    for (i = 0, j = left_index==0? 2:1; i < old->page.node_page.header.num_keys; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pnum[j] = old->page.node_page.kp_pair[i].pnum;
    }

    for (i = 0, j = 0; i < old->page.node_page.header.num_keys; i++, j++) {
        if (j == left_index) j++;
        temp_keys[j] = old->page.node_page.kp_pair[i].key;
    }

    temp_pnum[left_index + 1] = right_pnum;
    temp_keys[left_index] = key;

    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */
    int split = cut(MAX_INTERNAL_ORDER);
    old->page.node_page.one_more_page_pnum = temp_pnum[0];
    for (i = 0; i < split - 1; i++) {
        old->page.node_page.kp_pair[i].pnum = temp_pnum[++i];
        old->page.node_page.kp_pair[i].key = temp_keys[i];
    }
    old->page.node_page.header.num_keys = split - 1;
    k_prime = temp_keys[split - 1];

    new->page.node_page.one_more_page_pnum = temp_pnum[++i];
    for (j = 0; i < MAX_INTERNAL_ORDER; i++, j++) {
        new->page.node_page.kp_pair[j].pnum = temp_pnum[i+1];
        new->page.node_page.kp_pair[j].key = temp_keys[i];
    }
    new->page.node_page.header.num_keys = MAX_INTERNAL_ORDER - split;
    new->page.node_page.header.parent_pnum = old->page.node_page.header.parent_pnum;

    //file_read_page(new.node_page.one_more_page_pnum, &child);
    child = buf_read_page(tid, new->page.node_page.one_more_page_pnum, 1);
    child->page.node_page.header.parent_pnum = new->pnum;
    child->is_pinned = 0;
    //file_write_page(new.node_page.one_more_page_pnum, &child);

    for (i = 0; i < new->page.node_page.header.num_keys; i++) {
        //file_read_page(new.node_page.kp_pair[i].pnum, &child);
        child = buf_read_page(tid, new->page.node_page.kp_pair[i].pnum, 1);
        child->page.node_page.header.parent_pnum = new->pnum;
        //file_write_page(new.node_page.kp_pair[i].pnum, &child);
        child->is_pinned = 0;
    }


    /* Insert a new key into the parent of the two
     * nodes resulting from the split, with
     * the old node to the left and the new to the right.
     */

    pagenum_t new_root_pnum =  insert_into_parent(tid, buf_header, root_pnum, old_pnum, old, k_prime, new_pnum, new);
    new->is_pinned = 0;
    return new_root_pnum;
}



///* Inserts a new node (leaf or internal node) into the B+ tree.
// * Returns the root of the tree after insertion.
// */
pagenum_t insert_into_parent(int tid, frame_t* buf_header, pagenum_t root_pnum, pagenum_t left_pnum, frame_t* left, int64_t key, pagenum_t right_pnum, frame_t* right) {

    int left_index;
    frame_t* parent;
    pagenum_t parent_pnum = left->page.node_page.header.parent_pnum;

    /* Case: new root. */

    if(parent_pnum==0)
          return insert_into_new_root(tid, buf_header, left_pnum, left, key, right_pnum, right);

    /* Case: leaf or node. (Remainder of
     * function body.)
     */

    /* Find the parent's pointer to the left
     * node.
     */
    //file_read_page(parent_pnum, &parent);
    parent = buf_read_page(tid, parent_pnum, 1);
    left_index = get_left_index(parent, left->pnum);


    /* Simple case: the new key fits into the node.
     */
    int flag;
    if (parent->page.node_page.header.num_keys < MAX_INTERNAL_ORDER - 1){
         flag = insert_into_node(tid, root_pnum, parent, left_index, key, right->pnum);
         //file_write_page(left_pnum, left);
         //file_write_page(right_pnum, right);
         //file_write_page(parent_pnum, &parent);
         return root_pnum;
    }

    /* Harder case:  split a node in order
     * to preserve the B+ tree properties.
     */

    pagenum_t new_root_pnum =  insert_into_node_after_splitting(tid, buf_header, root_pnum, parent_pnum, left_index, key, right->pnum, parent);
    parent->is_pinned = 0;
    return new_root_pnum;
}


///* Creates a new root for two subtrees
// * and inserts the appropriate key into
// * the new root.
// */
pagenum_t insert_into_new_root(int tid, frame_t* buf_header, pagenum_t left_pnum, frame_t* left, int64_t key, pagenum_t right_pnum, frame_t* right) {

    frame_t* root = make_node(tid, buf_header);
    pagenum_t root_pnum = root->pnum;
    root->page.node_page.kp_pair[0].key = key;
    root->page.node_page.one_more_page_pnum = left->pnum;
    root->page.node_page.kp_pair[0].pnum = right->pnum;
    root->page.node_page.header.num_keys++;
    root->page.node_page.header.parent_pnum = 0;
    left->page.node_page.header.parent_pnum = root->pnum;
    right->page.node_page.header.parent_pnum = root->pnum;
    
    //file_write_page(root_pnum,root);
    //file_write_page(left_pnum,left);
    //file_write_page(right_pnum,right);
    root->is_pinned = 0;
    return root->pnum;
}



///* First insertion:
// * start a new tree.
// */
pagenum_t start_new_tree(int tid, frame_t* buf_header, int64_t key, char* value){

    frame_t* root = make_node(tid, buf_header);
    root->page.node_page.records[0].key = key;
    strcpy(root->page.node_page.records[0].value, value);
    root->page.node_page.header.is_leaf = 1;
    root->page.node_page.header.num_keys++;
    root->is_pinned = 0;
    
    //return root_pnum;
    return root->pnum;
}
//
//
//
///* Master insertion function.
// * Inserts a key and an associated value into
// * the B+ tree, causing the tree to be adjusted
// * however necessary to maintain the B+ tree
// * properties.
// */
int insert(int tid, int64_t key, char* value) {
   
   
   /* The current implementation ignores
     * duplicates.
     */
    char temp[VALUE_SIZE];
    if (find(tid, key, temp) == SUCCESS){
    	printf("ERR: Key %ld already exist!\n",key);
        return FAILURE;
    }
    
    frame_t* buf_header = buf_read_header(tid,1);


    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    
    if (buf_header->page.header_page.root_page_no == 0){
        buf_header->page.header_page.root_page_no = start_new_tree(tid, buf_header, key, value);
	 buf_header->is_pinned = 0;
        return SUCCESS;
      }
      
    /* Case: the tree already exists.
     * (Rest of function body.)
     */

    pagenum_t pnum = find_leaf(tid, buf_header->page.header_page.root_page_no, key);
    frame_t* p;
    p = buf_read_page(tid, pnum, 1);
    
    /* Case: leaf has room for key and pointer.
     */

    if (p->page.node_page.header.num_keys < MAX_LEAF_ORDER - 1) {
        insert_into_leaf(tid, pnum, p, key, value);
        buf_header->is_pinned = 0;
        p->is_pinned = 0;
        return SUCCESS;
    }


    /* Case:  leaf must be split.
     */

    
    buf_header->page.header_page.root_page_no = insert_into_leaf_after_splitting(tid, buf_header, buf_header->page.header_page.root_page_no, pnum, p , key, value);
    buf_header->is_pinned = 0;
    p->is_pinned = 0;
    return SUCCESS;
}





//// DELETION.
//
///* Utility function for deletion.  Retrieves
// * the index of a node's nearest neighbor (sibling)
// * to the left if one exists.  If not (the node
// * is the leftmost child), returns -1 to signify
// * this special case.
// */
int get_neighbor_index( frame_t* parent, pagenum_t current_pnum ) {

    int i;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.
     * If n is the leftmost child, this means
     * return -1.
     */
    for (i = 0; i <= parent->page.node_page.header.num_keys; i++)
        if (parent->page.node_page.kp_pair[i].pnum == current_pnum)
            return i;

    // Error state.
    //printf("Search for nonexistent pointer to node in parent.\n");
    //printf("Node:  %#lx\n", (unsigned long)n);
    //exit(EXIT_FAILURE);
    
    if(parent->page.node_page.one_more_page_pnum == current_pnum) 
        return -1;
}
//
//
int remove_entry_from_node(int64_t key, frame_t* node, int type){
 
    int i;
    if(type == LEAF){
	// Remove the key and shift other keys accordingly.
	    i = 0;
	    while (node->page.node_page.records[i].key != key)
		i++;
	    for (++i; i < node->page.node_page.header.num_keys; i++){
		strcpy(node->page.node_page.records[i-1].value, node->page.node_page.records[i].value);
		node->page.node_page.records[i-1].key = node->page.node_page.records[i].key;
	    } node->page.node_page.header.num_keys--;
	    return SUCCESS;
    }
    else if(type == INTERNAL){
	    i = 0;
	    while (node->page.node_page.kp_pair[i].key != key)
		i++;
	    for (++i; i < node->page.node_page.header.num_keys; i++){
		node->page.node_page.kp_pair[i-1].pnum = node->page.node_page.kp_pair[i].pnum;
		node->page.node_page.kp_pair[i-1].key = node->page.node_page.kp_pair[i].key;
	    } node->page.node_page.header.num_keys--;
	    return SUCCESS;
    }
    return FAILURE;
}
//
//
//returns new root page number after deletion
pagenum_t adjust_root(int tid, frame_t* buf_header, pagenum_t old_root_pnum){

    frame_t* old_root;
    old_root = buf_read_page(tid, old_root_pnum, 1);
    
    frame_t* new_root;
    pagenum_t new_root_pnum;

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */

    if (old_root->page.node_page.header.num_keys > 0)
        return old_root_pnum;

    /* Case: empty root. 
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.

    if (!old_root->page.node_page.header.is_leaf) {
        new_root_pnum = old_root->page.node_page.one_more_page_pnum;
        //file_read_page(new_root_pnum, &new_root);
        new_root = buf_read_page(tid, new_root_pnum, 1);
        new_root->page.node_page.header.parent_pnum = 0;
        new_root->is_pinned = 0;
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else
        new_root_pnum = 0;

    //free(root->keys);
    //free(root->pointers);
    //free(root);
    buf_free_page(tid, buf_header, old_root, old_root_pnum);
    old_root->is_pinned = 0;
    return new_root_pnum;
}


///* Coalesces a node that has become
// * too small after deletion
// * with a neighboring node that
// * can accept the additional entries
// * without exceeding the maximum.
// */
pagenum_t coalesce_nodes(int tid, frame_t* buf_header, pagenum_t root_pnum, pagenum_t current_pnum, frame_t* current, pagenum_t neighbor_pnum, frame_t* neighbor, int neighbor_index, int64_t k_prime){

    int i, j, neighbor_insertion_index, n_end;
    frame_t* tmp;
    pagenum_t tmp_pnum;

    /* Swap neighbor with node if node is on the
     * extreme left and neighbor is to its right.
     */

    if (neighbor_index == -1) {
        tmp = current;
        //tmp_pnum = current_pnum;
        
        current = neighbor;
        //current_pnum = neighbor_pnum;
        
        neighbor = tmp;
        //neighbor_pnum = tmp_pnum;
    }

    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */

    neighbor_insertion_index = neighbor->page.node_page.header.num_keys;

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

    if (!current->page.node_page.header.is_leaf) {

        /* Append k_prime.
         */

        neighbor->page.node_page.kp_pair[neighbor_insertion_index].key = k_prime;
        neighbor->page.node_page.kp_pair[neighbor_insertion_index].pnum = current->page. node_page.one_more_page_pnum;
        
        neighbor->page.node_page.header.num_keys++;
        
        n_end = current->page.node_page.header.num_keys;

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            neighbor->page.node_page.kp_pair[i].key = current->page.node_page.kp_pair[j].key;
            neighbor->page.node_page.kp_pair[i].pnum = current->page.node_page.kp_pair[j].pnum;
            //neighbor->num_keys++;
            //n->num_keys--;
        }

        /* The number of pointers is always
         * one more than the number of keys.
         */

        neighbor->page.node_page.header.num_keys += n_end;

        /* All children must now point up to the same parent.
         */
	//frame_t* child;
	//pagenum_t child_pnum = neighbor->page.node_page.kp_pair[i].pnum;
        for (i=neighbor_insertion_index; i<neighbor->page.node_page.header.num_keys; i++) {
            tmp = buf_read_page(tid, neighbor->page.node_page.kp_pair[i].pnum, 1);
            tmp->page.node_page.header.parent_pnum = neighbor->pnum;
            tmp->is_pinned = 0;
        }
    }

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
        for (i = neighbor_insertion_index, j = 0; j < current->page.node_page.header.num_keys; i++, j++) {
            neighbor->page.node_page.records[i].key = current->page.node_page.records[j].key;
            strcpy(neighbor->page.node_page.records[i].value, current->page.node_page.records[j].value);
            //neighbor->num_keys++;
        }
        neighbor->page.node_page.header.num_keys += current->page.node_page.header.num_keys;
        neighbor->page.node_page.right_sibling_pnum = current->page.node_page.right_sibling_pnum;
    }

    
    //root = delete_entry(root, n->parent, k_prime, n);
    root_pnum = delete_entry(tid, buf_header, root_pnum, current->page.node_page.header.parent_pnum, k_prime);
    
    //free(n->keys);
    //free(n->pointers);
    //free(n);
    buf_free_page(tid, buf_header, current, current_pnum);
    return root_pnum;
}
//
//
///* Redistributes entries between two nodes when
// * one has become too small after deletion
// * but its neighbor is too big to append the
// * small node's entries without exceeding the
// * maximum
// */
int redistribute_nodes(int tid, frame_t* buf_header, pagenum_t root_pnum, pagenum_t current_pnum, frame_t* current, pagenum_t neighbor_pnum, frame_t* neighbor, int neighbor_index, int64_t k_prime, int k_prime_index){

    int i,num_keys;
    frame_t *tmp, *parent;
    pagenum_t tmp_pnum, parent_pnum;
    parent_pnum = current->page.node_page.header.parent_pnum;
    parent = buf_read_page(tid, parent_pnum, 1);
    num_keys = current->page.node_page.header.num_keys;
    tmp_pnum = current->page.node_page.kp_pair[num_keys].pnum;


    /* Case: n has a neighbor to the left.
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */
  
    if (neighbor_index != -1) {
        current->page.node_page.kp_pair[num_keys].pnum = neighbor->page.node_page.one_more_page_pnum;    
        current->page.node_page.kp_pair[num_keys].key = k_prime;
        tmp = buf_read_page(tid, tmp_pnum, 1);
        tmp->page.node_page.header.parent_pnum = current->pnum;
        tmp->is_pinned = 0;
        parent->page.node_page.kp_pair[k_prime_index].key = neighbor->page.node_page.kp_pair[0].key;
        neighbor->page.node_page.one_more_page_pnum = neighbor->page.node_page.kp_pair[0].pnum;
        
        for (i=0; i<neighbor->page.node_page.header.num_keys -1; i++) {
            neighbor->page.node_page.kp_pair[i].pnum = neighbor->page.node_page.kp_pair[i+1].pnum;
            neighbor->page.node_page.kp_pair[i].key = neighbor->page.node_page.kp_pair[i+1].key;
            
        }
    }

    /* Case: n is the leftmost child.
     * Take a key-pointer pair from the neighbor to the right.
     * Move the neighbor's leftmost key-pointer pair
     * to n's rightmost position.
     */

    else {
        for (i=current->page.node_page.header.num_keys; i > 0; i--) {
            current->page.node_page.kp_pair[i].pnum = current->page.node_page.kp_pair[i-1].pnum;
            current->page.node_page.kp_pair[i].key = current->page.node_page.kp_pair[i-1].key;
            
        }
        int ng_keys = neighbor->page.node_page.header.num_keys;
        current->page.node_page.kp_pair[0].pnum = current->page.node_page.one_more_page_pnum;
        current->page.node_page.one_more_page_pnum = neighbor->page.node_page.kp_pair[ng_keys-1].pnum;
        
        tmp = buf_read_page(tid, current->page.node_page.one_more_page_pnum,1);
        tmp->page.node_page.header.parent_pnum = current->pnum;
        tmp->is_pinned = 0;
        current->page.node_page.kp_pair[0].key = k_prime;
        parent->page.node_page.kp_pair[k_prime_index].key = neighbor->page.node_page.kp_pair[ng_keys-1].key;
    }

    /* n now has one more key and one more pointer;
     * the neighbor has one fewer of each.
     */

    //n->num_keys++;
    //neighbor->num_keys--;
    parent->is_pinned = 0;
    current->page.node_page.header.num_keys++;
    neighbor->page.node_page.header.num_keys--;
    
    return SUCCESS;
}
//
//
///* Deletes an entry from the B+ tree.
// * Removes the record and its key and pointer
// * from the leaf, and then makes all appropriate
// * changes to preserve the B+ tree properties.
// * Returns new root page number/
pagenum_t delete_entry(int tid, frame_t* buf_header, pagenum_t root_pnum, pagenum_t current_pnum, int64_t key ) {

    int min_keys;
    int neighbor_index;
    int k_prime_index; 
    int64_t k_prime;
    int capacity,flag;
    pagenum_t res;

    frame_t* current = buf_read_page(tid, current_pnum, 1);
    
    capacity = current->page.node_page.header.is_leaf ? MAX_LEAF_ORDER : MAX_INTERNAL_ORDER-1;
        
    // Remove key and pointer from node.

    int type = current->page.node_page.header.is_leaf;
    flag = remove_entry_from_node(key, current, type);

    /* Case:  deletion from the root.
     */

    if (current_pnum == root_pnum){
    	current->is_pinned = 0;
        return adjust_root(tid, buf_header, root_pnum);
    }


    /* Case:  deletion from a node below the root.
     * (Rest of function body.)
     */

    /* Determine minimum allowable size of node,
     * to be preserved after deletion.
     */

    min_keys = 1;

    /* Case:  node stays at or above minimum.
     * (The simple case.)
     */

    if (current->page.node_page.header.num_keys >= min_keys){
        current->is_pinned = 0;
        return root_pnum;
    }

    /* Case:  node falls below minimum.
     * Either coalescence or redistribution
     * is needed.
     */

    /* Find the appropriate neighbor node with which
     * to coalesce.
     * Also find the key (k_prime) in the parent
     * between the pointer to node n and the pointer
     * to the neighbor.
     */
    frame_t* parent, *neighbor;
    pagenum_t parent_pnum, neighbor_pnum;
    
    parent_pnum = current->page.node_page.header.parent_pnum;
    parent = buf_read_page(tid, parent_pnum, 1);
    
    neighbor_index = get_neighbor_index(parent, current_pnum);
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
    k_prime = parent->page.node_page.kp_pair[k_prime_index].key;
    if(neighbor_index == 0)
    	neighbor_pnum = parent->page.node_page.one_more_page_pnum;
    else if (neighbor_index == -1)
    	neighbor_pnum = parent->page.node_page.kp_pair[0].pnum;
    else
    	neighbor_pnum = parent->page.node_page.kp_pair[neighbor_index-1].pnum;
    parent->is_pinned = 0;
    
    neighbor = buf_read_page(tid, neighbor_pnum, 1);
    /* Coalescence. */

    if (neighbor->page.node_page.header.num_keys + current->page.node_page.header.num_keys < capacity){
        res = coalesce_nodes(tid, buf_header, root_pnum, current_pnum, current, neighbor_pnum, neighbor, neighbor_index, k_prime);
        current->is_pinned = 0;
        neighbor->is_pinned = 0;
        return res;
    }

    /* Redistribution. */

    else{
        res = redistribute_nodes(tid, buf_header, root_pnum, current_pnum, current, neighbor_pnum, neighbor, neighbor_index, k_prime, k_prime_index);
        current->is_pinned = 0;
        neighbor->is_pinned = 0;
        return res;
    }
}
//
//
//
///* Master deletion function.
// */
int delete(int tid, int64_t key){
    char temp_val[VALUE_SIZE];
    
    if( find(tid,key,temp_val) == FAILURE){
    	printf("ERR: Key %ld not found!\n", key);
    	return FAILURE;
    }
    
    frame_t* buf_header = buf_read_header(tid, 1);
    pagenum_t key_leaf = find_leaf(tid, buf_header->page.header_page.root_page_no, key);
    
    if(key_leaf == 0){
    	printf("ERR: Tree is empty!\n");
    	return FAILURE;
    }

    //get new root page number after deleting
    buf_header->page.header_page.root_page_no = delete_entry(tid, buf_header, buf_header->page.header_page.root_page_no, key_leaf, key);

    buf_header->is_pinned = 0;
    
    return SUCCESS;
}
