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
pagenum_t find_leaf(pagenum_t root, int64_t key){
    int i = 0;
    pagenum_t pnum = root;
    page_t p;
    file_read_page(pnum,&p);
    if (pnum == 0) {
        printf("Empty tree.\n");
        return pnum;
    }
    while (!p.node_page.header.is_leaf) {
        i = 0;
        while (i < p.node_page.header.num_keys) {
            if (key >= p.node_page.kp_pair[i].key) i++;
            else break;
        }
        if (i==0)
            pnum = p.node_page.one_more_page_pnum;
        else
            pnum = p.node_page.kp_pair[i-1].pnum;
        file_read_page(pnum,&p);
    }
    return pnum;
}
//
//
///* Finds and returns the record to which
// * a key refers.
// */
int find( int64_t key, char* value) {
    file_read_header();
    int i = 0;
    pagenum_t c = find_leaf(main_header.header_page.root_page_no , key);
    page_t leaf;
    if (c == 0){
     return FAILURE;
     }
     
    file_read_page(c,&leaf);
    for (i = 0; i < leaf.node_page.header.num_keys; i++){
        if (leaf.node_page.records[i].key == key) break;
    }
    if (i == leaf.node_page.header.num_keys){
        return FAILURE;
       }
    else
        strcpy(value,leaf.node_page.records[i].value);
        return SUCCESS;
}
//
///* Finds the appropriate place to
// * split a node that is too big into two.
// */
int cut( int length ) {
    if (length % 2 == 0)
        return length/2;
    else
        return length/2 + 1;
}
//
//
//// INSERTION
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
//
//
///* Creates a new general node, which can be adapted
// * to serve as either a leaf or an internal node.
// */
pagenum_t make_node(page_t* p ) {
    pagenum_t pnum = file_alloc_page();
    memset(p,0,PAGE_SIZE);
    return pnum;
}

///* Creates a new leaf by creating a node
// * and then adapting it appropriately.
// */
pagenum_t make_leaf(page_t* leaf ) {
    pagenum_t leaf_pnum = make_node(leaf);
    leaf->node_page.header.is_leaf = 1;
    return leaf_pnum;
}


///* Helper function used in insert_into_parent
// * to find the index of the parent's pointer to
// * the node to the left of the key to be inserted.
// */
int get_left_index(page_t* parent, pagenum_t left_pnum) {
    if(parent->node_page.one_more_page_pnum == left_pnum) return 0;
    int left_index = 1;
    while (left_index <= parent->node_page.header.num_keys &&
            parent->node_page.kp_pair[left_index-1].pnum != left_pnum)
        left_index++;
    return left_index;
}

///* Inserts a new pointer to a record and its corresponding
// * key into a leaf.
// */
int insert_into_leaf(pagenum_t leaf_pnum, page_t* leaf, int64_t key, char* value ){
    int insertion_point;

    insertion_point = 0;
    while (insertion_point < leaf->node_page.header.num_keys && leaf-> node_page.records[insertion_point].key < key)
        insertion_point++;

    for (int i = leaf->node_page.header.num_keys; i > insertion_point; i--) {
        leaf->node_page.records[i].key = leaf->node_page.records[i-1].key;
        strcpy(leaf->node_page.records[i].value, leaf->node_page.records[i-1].value);
    }
    leaf->node_page.records[insertion_point].key = key;
    strcpy(leaf->node_page.records[insertion_point].value, value);
    leaf->node_page.header.num_keys++;
    file_write_page(leaf_pnum, leaf);
    return SUCCESS;
}
//
//
///* Inserts a new key and pointer
// * to a new record into a leaf so as to exceed
// * the tree's order, causing the leaf to be split
// * in half.
// */
pagenum_t insert_into_leaf_after_splitting(pagenum_t root_pnum, pagenum_t leaf_pnum, page_t* leaf, int64_t key, char* value) {

    page_t new_leaf;
    pagenum_t new_leaf_pnum;
    int temp_keys[MAX_LEAF_ORDER];
    char temp_val[MAX_LEAF_ORDER][VALUE_SIZE];
    int insertion_index, split, new_key, i, j;

    new_leaf_pnum = make_leaf(&new_leaf);

    insertion_index = 0;
    while (insertion_index < MAX_LEAF_ORDER - 1 && leaf->node_page.records[insertion_index].key < key)
        insertion_index++;

    for (i = 0, j = 0; i < leaf->node_page.header.num_keys; i++, j++) {
        if (j == insertion_index) j++;
        temp_keys[j] = leaf->node_page.records[i].key;
        strcpy(temp_val[j], leaf->node_page.records[i].value);
    }
    temp_keys[insertion_index] = key;
    strcpy(temp_val[insertion_index] , value);

    split = cut(MAX_LEAF_ORDER - 1);
    
    for(i = 0; i < split; i++) {
        leaf->node_page.records[i].key = temp_keys[i];
        strcpy(leaf->node_page.records[i].value, temp_val[i]);
        
    }
    leaf->node_page.header.num_keys = split;

    for (i = split, j = 0; i < MAX_LEAF_ORDER; i++, j++) {
        new_leaf.node_page.records[j].key = temp_keys[i];
        strcpy(new_leaf.node_page.records[j].value, temp_val[i]);
        
    }
    new_leaf.node_page.header.num_keys = MAX_LEAF_ORDER - split;

    new_leaf.node_page.right_sibling_pnum = leaf->node_page.right_sibling_pnum;
    leaf->node_page.right_sibling_pnum = new_leaf_pnum;
    new_leaf.node_page.header.parent_pnum = leaf->node_page.header.parent_pnum;
    new_key = new_leaf.node_page.records[0].key;

    file_write_page(leaf_pnum, leaf);
    file_write_page(new_leaf_pnum, &new_leaf);

    return insert_into_parent(root_pnum, leaf_pnum, leaf, new_key, new_leaf_pnum, &new_leaf);
}
///* Inserts a new key and pointer to a node
// * into a node into which these can fit
// * without violating the B+ tree properties.
// */
int insert_into_node(pagenum_t root_pnum, page_t* n,int left_index, int64_t key, pagenum_t right_pnum){
    int i;
    
    for (i = n->node_page.header.num_keys; i > left_index; i--) {
        n->node_page.kp_pair[i].key = n->node_page.kp_pair[i-1].key;
        n->node_page.kp_pair[i].pnum = n->node_page.kp_pair[i-1].pnum;
    }
    n->node_page.kp_pair[left_index].key = key;
    n->node_page.kp_pair[left_index].pnum = right_pnum;
    n->node_page.header.num_keys++;
    return SUCCESS;
}
//
//
///* Inserts a new key and pointer to a node
// * into a node, causing the node's size to exceed
// * the order, and causing the node to split into two.
// */
pagenum_t insert_into_node_after_splitting(pagenum_t root_pnum, pagenum_t old_pnum,
                                        int left_index, int64_t key, pagenum_t right_pnum,page_t* old)
                                        {

    int i, j;
    int64_t k_prime;
    page_t new, child;
    pagenum_t new_pnum;
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
    temp_pnum[0] = old->node_page.one_more_page_pnum; 
    for (i = 0, j = left_index==0? 2:1; i < old->node_page.header.num_keys; i++, j++) {
        if (j == left_index + 1) j++;
        temp_pnum[j] = old->node_page.kp_pair[i].pnum;
    }

    for (i = 0, j = 0; i < old->node_page.header.num_keys; i++, j++) {
        if (j == left_index) j++;
        temp_keys[j] = old->node_page.kp_pair[i].key;
    }

    temp_pnum[left_index + 1] = right_pnum;
    temp_keys[left_index] = key;

    /* Create the new node and copy
     * half the keys and pointers to the
     * old and half to the new.
     */
    int split = cut(MAX_INTERNAL_ORDER);
    new_pnum = make_node(&new);
    old->node_page.one_more_page_pnum = temp_pnum[0];
    for (i = 0; i < split - 1; i++) {
        old->node_page.kp_pair[i].pnum = temp_pnum[++i];
        old->node_page.kp_pair[i].key = temp_keys[i];
    }
    old->node_page.header.num_keys = split - 1;
    k_prime = temp_keys[split - 1];

    new.node_page.one_more_page_pnum = temp_pnum[++i];
    for (j = 0; i < MAX_INTERNAL_ORDER; i++, j++) {
        new.node_page.kp_pair[j].pnum = temp_pnum[++i];
        new.node_page.kp_pair[j].key = temp_keys[i];
    }
    new.node_page.header.num_keys = MAX_INTERNAL_ORDER - split;
    new.node_page.header.parent_pnum = old->node_page.header.parent_pnum;

    file_read_page(new.node_page.one_more_page_pnum, &child);
    child.node_page.header.parent_pnum = new_pnum;
    file_write_page(new.node_page.one_more_page_pnum, &child);

    for (i = 0; i < new.node_page.header.num_keys; i++) {
        file_read_page(new.node_page.kp_pair[i].pnum, &child);
        child.node_page.header.parent_pnum = new_pnum;
        file_write_page(new.node_page.kp_pair[i].pnum, &child);
    }


    /* Insert a new key into the parent of the two
     * nodes resulting from the split, with
     * the old node to the left and the new to the right.
     */

    return insert_into_parent(root_pnum, old_pnum, old, k_prime, new_pnum, &new);
}
//
//
//
///* Inserts a new node (leaf or internal node) into the B+ tree.
// * Returns the root of the tree after insertion.
// */
pagenum_t insert_into_parent(pagenum_t root_pnum, pagenum_t left_pnum, page_t* left, int64_t key, pagenum_t right_pnum, page_t* right) {

    int left_index;
    page_t parent;
    pagenum_t parent_pnum = left->node_page.header.parent_pnum;

    /* Case: new root. */

    //if (parent == NULL)
    //    return insert_into_new_root(left, key, right);
    if(parent_pnum==0)
          return insert_into_new_root(left_pnum, left, key, right_pnum, right);

    /* Case: leaf or node. (Remainder of
     * function body.)
     */

    /* Find the parent's pointer to the left
     * node.
     */
    file_read_page(parent_pnum, &parent);
    left_index = get_left_index(&parent, left_pnum);


    /* Simple case: the new key fits into the node.
     */
    int flag;
    if (parent.node_page.header.num_keys < MAX_INTERNAL_ORDER - 1){
         flag = insert_into_node(root_pnum, &parent, left_index, key, right_pnum);
         file_write_page(left_pnum, left);
         file_write_page(right_pnum, right);
         file_write_page(parent_pnum, &parent);
         return root_pnum;
    }

    /* Harder case:  split a node in order
     * to preserve the B+ tree properties.
     */

    return insert_into_node_after_splitting(root_pnum, parent_pnum, left_index, key, right_pnum, &parent);
}


///* Creates a new root for two subtrees
// * and inserts the appropriate key into
// * the new root.
// */
pagenum_t insert_into_new_root(pagenum_t left_pnum, page_t* left, int64_t key, pagenum_t right_pnum, page_t* right) {

    page_t* root;
    pagenum_t root_pnum = make_node(root);
    root->node_page.kp_pair[0].key = key;
    root->node_page.one_more_page_pnum = left_pnum;
    root->node_page.kp_pair[0].pnum = right_pnum;
    root->node_page.header.num_keys++;
    root->node_page.header.parent_pnum = 0;
    left->node_page.header.parent_pnum = root_pnum;
    right->node_page.header.parent_pnum = root_pnum;
    
    file_write_page(root_pnum,root);
    file_write_page(left_pnum,left);
    file_write_page(right_pnum,right);
    return root_pnum;
}
//
//
//
///* First insertion:
// * start a new tree.
// */
pagenum_t start_new_tree(int64_t key, char* value){

    page_t root;
    pagenum_t root_pnum = make_node(&root);
    root.node_page.records[0].key = key;
    strcpy(root.node_page.records[0].value, value);
    root.node_page.header.is_leaf = 1;
    root.node_page.header.num_keys++;
    file_write_page(root_pnum,&root);
    return root_pnum;
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
int insert(int64_t key, char* value) {
   
   file_read_header();
   
   /* The current implementation ignores
     * duplicates.
     */
    char temp[VALUE_SIZE];
    if (find(key, temp) == SUCCESS){
    	printf("ERR: Key %ld already exist!\n",key);
        return FAILURE;
    }


    /* Case: the tree does not exist yet.
     * Start a new tree.
     */

    if (main_header.header_page.root_page_no == 0){
        main_header.header_page.root_page_no = start_new_tree(key, value);
	  file_write_header();
        return SUCCESS;
      }
      
    /* Case: the tree already exists.
     * (Rest of function body.)
     */

    pagenum_t pnum = find_leaf(main_header.header_page.root_page_no, key);
    page_t p;
    file_read_page(pnum,&p);
    
    /* Case: leaf has room for key and pointer.
     */

    if (p.node_page.header.num_keys < MAX_LEAF_ORDER - 1) {
        insert_into_leaf(pnum, &p, key, value);
        return SUCCESS;
    }


    /* Case:  leaf must be split.
     */

    
    main_header.header_page.root_page_no = insert_into_leaf_after_splitting(main_header.header_page.root_page_no, pnum, &p , key, value);
    file_write_header();
    return SUCCESS;
}
//
//
//
//
//// DELETION.
//
///* Utility function for deletion.  Retrieves
// * the index of a node's nearest neighbor (sibling)
// * to the left if one exists.  If not (the node
// * is the leftmost child), returns -1 to signify
// * this special case.
// */
int get_neighbor_index( page_t* parent, pagenum_t current_pnum ) {

    int i;

    /* Return the index of the key to the left
     * of the pointer in the parent pointing
     * to n.
     * If n is the leftmost child, this means
     * return -1.
     */
    for (i = 0; i <= parent->node_page.header.num_keys; i++)
        if (parent->node_page.kp_pair[i].pnum == current_pnum)
            return i;

    // Error state.
    //printf("Search for nonexistent pointer to node in parent.\n");
    //printf("Node:  %#lx\n", (unsigned long)n);
    //exit(EXIT_FAILURE);
    
    if(parent->node_page.one_more_page_pnum == current_pnum) 
        return -1;
}
//
//
int remove_entry_from_node(int64_t key, page_t* node, int type){
 
    int i;
    if(type == LEAF){
	// Remove the key and shift other keys accordingly.
	    i = 0;
	    while (node->node_page.records[i].key != key)
		i++;
	    for (++i; i < node->node_page.header.num_keys; i++){
		strcpy(node->node_page.records[i-1].value, node->node_page.records[i].value);
		node->node_page.records[i-1].key = node->node_page.records[i].key;
	    } node->node_page.header.num_keys--;
	    return SUCCESS;
    }
    else if(type == INTERNAL){
	    i = 0;
	    while (node->node_page.kp_pair[i].key != key)
		i++;
	    for (++i; i < node->node_page.header.num_keys; i++){
		node->node_page.kp_pair[i-1].pnum = node->node_page.kp_pair[i].pnum;
		node->node_page.kp_pair[i-1].key = node->node_page.kp_pair[i].key;
	    } node->node_page.header.num_keys--;
	    return SUCCESS;
    }
    return FAILURE;
}
//
//
//returns new root page number after deletion
pagenum_t adjust_root(pagenum_t old_root_pnum) {

    page_t old_root;
    file_read_page(old_root_pnum, &old_root);
    
    page_t new_root;
    pagenum_t new_root_pnum;

    /* Case: nonempty root.
     * Key and pointer have already been deleted,
     * so nothing to be done.
     */

    if (old_root.node_page.header.num_keys > 0)
        return old_root_pnum;

    /* Case: empty root. 
     */

    // If it has a child, promote 
    // the first (only) child
    // as the new root.

    if (!old_root.node_page.header.is_leaf) {
        new_root_pnum = old_root.node_page.one_more_page_pnum;
        file_read_page(new_root_pnum, &new_root);
        new_root.node_page.header.parent_pnum = 0;
        file_write_page(new_root_pnum, &new_root);
    }

    // If it is a leaf (has no children),
    // then the whole tree is empty.

    else
        new_root_pnum = 0;

    //free(root->keys);
    //free(root->pointers);
    //free(root);
    file_free_page(old_root_pnum);
    return new_root_pnum;
}


///* Coalesces a node that has become
// * too small after deletion
// * with a neighboring node that
// * can accept the additional entries
// * without exceeding the maximum.
// */
pagenum_t coalesce_nodes(pagenum_t root_pnum, pagenum_t current_pnum, page_t* current, pagenum_t neighbor_pnum, page_t* neighbor, int neighbor_index,int64_t k_prime){

    int i, j, neighbor_insertion_index, n_end;
    page_t* tmp;
    pagenum_t tmp_pnum;

    /* Swap neighbor with node if node is on the
     * extreme left and neighbor is to its right.
     */

    if (neighbor_index == -1) {
        tmp = current;
        tmp_pnum = current_pnum;
        
        current = neighbor;
        current_pnum = neighbor_pnum;
        
        neighbor = tmp;
        neighbor_pnum = tmp_pnum;
    }

    /* Starting point in the neighbor for copying
     * keys and pointers from n.
     * Recall that n and neighbor have swapped places
     * in the special case of n being a leftmost child.
     */

    neighbor_insertion_index = neighbor->node_page.header.num_keys;

    /* Case:  nonleaf node.
     * Append k_prime and the following pointer.
     * Append all pointers and keys from the neighbor.
     */

    if (!current->node_page.header.is_leaf) {

        /* Append k_prime.
         */

        neighbor->node_page.kp_pair[neighbor_insertion_index].key = k_prime;
        neighbor->node_page.kp_pair[neighbor_insertion_index].pnum = current-> node_page.one_more_page_pnum;
        
        neighbor->node_page.header.num_keys++;
        
        n_end = current->node_page.header.num_keys;

        for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
            neighbor->node_page.kp_pair[i].key = current->node_page.kp_pair[j].key;
            neighbor->node_page.kp_pair[i].pnum = current->node_page.kp_pair[j].pnum;
            //neighbor->num_keys++;
            //n->num_keys--;
        }

        /* The number of pointers is always
         * one more than the number of keys.
         */

        neighbor->node_page.header.num_keys += n_end;

        /* All children must now point up to the same parent.
         */
	page_t child;
	pagenum_t child_pnum = neighbor->node_page.kp_pair[i].pnum;
        for (i=neighbor_insertion_index; i<neighbor->node_page.header.num_keys; i++) {
            file_read_page(child_pnum, &child);
            child.node_page.header.parent_pnum = neighbor_pnum;
            file_write_page(child_pnum, &child);
        }
    }

    /* In a leaf, append the keys and pointers of
     * n to the neighbor.
     * Set the neighbor's last pointer to point to
     * what had been n's right neighbor.
     */

    else {
        for (i = neighbor_insertion_index, j = 0; j < current->node_page.header.num_keys; i++, j++) {
            neighbor->node_page.records[i].key = current->node_page.records[j].key;
            strcpy(neighbor->node_page.records[i].value, current->node_page.records[j].value);
            //neighbor->num_keys++;
        }
        neighbor->node_page.header.num_keys += current->node_page.header.num_keys;
        neighbor->node_page.right_sibling_pnum += current->node_page.right_sibling_pnum;
    }

    file_write_page(neighbor_pnum, neighbor);
    
    //root = delete_entry(root, n->parent, k_prime, n);
    root_pnum = delete_entry(root_pnum, current->node_page.header.parent_pnum, k_prime);
    
    //free(n->keys);
    //free(n->pointers);
    //free(n);
    file_free_page(current_pnum);
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
int redistribute_nodes(pagenum_t root_pnum, pagenum_t current_pnum, page_t* current, pagenum_t neighbor_pnum, page_t* neighbor, int neighbor_index, int64_t k_prime, int k_prime_index){

    int i,num_keys;
    page_t tmp, parent;
    pagenum_t tmp_pnum, parent_pnum;
    parent_pnum = current->node_page.header.parent_pnum;
    file_read_page(parent_pnum, &parent);
    num_keys = current->node_page.header.num_keys;


    /* Case: n has a neighbor to the left.
     * Pull the neighbor's last key-pointer pair over
     * from the neighbor's right end to n's left end.
     */
  
    if (neighbor_index != -1) {
        current->node_page.kp_pair[num_keys].pnum = neighbor->node_page.one_more_page_pnum;    
        current->node_page.kp_pair[num_keys].key = k_prime;
        file_read_page(num_keys, &tmp);
        tmp.node_page.header.parent_pnum = current_pnum;
        file_write_page(num_keys, &tmp);
        parent.node_page.kp_pair[k_prime_index].key = neighbor->node_page.kp_pair[0].key;
        neighbor->node_page.one_more_page_pnum = neighbor->node_page.kp_pair[0].pnum;
        
        for (i=0; i<neighbor->node_page.header.num_keys -1; i++) {
            neighbor->node_page.kp_pair[i].pnum = neighbor->node_page.kp_pair[i+1].pnum;
            neighbor->node_page.kp_pair[i].key = neighbor->node_page.kp_pair[i+1].key;
            
        }
    }

    /* Case: n is the leftmost child.
     * Take a key-pointer pair from the neighbor to the right.
     * Move the neighbor's leftmost key-pointer pair
     * to n's rightmost position.
     */

    else {
        for (i=current->node_page.header.num_keys; i > 0; i--) {
            current->node_page.kp_pair[i].pnum = current->node_page.kp_pair[i-1].pnum;
            current->node_page.kp_pair[i].key = current->node_page.kp_pair[i-1].key;
            
        }
        int ng_keys = neighbor->node_page.header.num_keys;
        current->node_page.kp_pair[0].pnum = current->node_page.one_more_page_pnum;
        current->node_page.one_more_page_pnum = neighbor->node_page.kp_pair[ng_keys-1].pnum;
        
        file_read_page(current->node_page.one_more_page_pnum, &tmp);
        tmp.node_page.header.parent_pnum = current_pnum;
        file_write_page(current->node_page.one_more_page_pnum, &tmp);
        current->node_page.kp_pair[0].key = k_prime;
        parent.node_page.kp_pair[k_prime_index].key = neighbor->node_page.kp_pair[ng_keys-1].key;
    }

    /* n now has one more key and one more pointer;
     * the neighbor has one fewer of each.
     */

    //n->num_keys++;
    //neighbor->num_keys--;
    current->node_page.header.num_keys++;
    neighbor->node_page.header.num_keys--;
    
    file_write_page(current_pnum, current);
    file_write_page(neighbor_pnum, neighbor);
    file_write_page(current->node_page.header.parent_pnum, &parent);

    return SUCCESS;
}
//
//
///* Deletes an entry from the B+ tree.
// * Removes the record and its key and pointer
// * from the leaf, and then makes all appropriate
// * changes to preserve the B+ tree properties.
// * Returns new root page number/
pagenum_t delete_entry( pagenum_t root_pnum, pagenum_t current_pnum, int64_t key ) {

    int min_keys;
    int neighbor_index;
    int k_prime_index; 
    int64_t k_prime;
    int capacity,flag;
    
    

    page_t current;
    file_read_page(current_pnum, &current);
    capacity = current.node_page.header.is_leaf ? MAX_LEAF_ORDER : MAX_INTERNAL_ORDER-1;
    // Remove key and pointer from node.

    int type = current.node_page.header.is_leaf;
    flag = remove_entry_from_node(key, &current, type);
    file_write_page(current_pnum, &current);

    /* Case:  deletion from the root.
     */

    if (current_pnum == root_pnum)
        return adjust_root(root_pnum);


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

    if (current.node_page.header.num_keys >= min_keys)
        return root_pnum;

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
    page_t parent, neighbor;
    pagenum_t parent_pnum, neighbor_pnum;
    
    parent_pnum = current.node_page.header.parent_pnum;
    file_read_page(parent_pnum,&parent);
    
    neighbor_index = get_neighbor_index(&parent, current_pnum);
    k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
    k_prime = parent.node_page.kp_pair[k_prime_index].key;
    if(neighbor_index == 0)
    	neighbor_pnum = parent.node_page.one_more_page_pnum;
    else if (neighbor_index == -1)
    	neighbor_pnum = parent.node_page.kp_pair[0].pnum;
    else
    	neighbor_pnum = parent.node_page.kp_pair[neighbor_index-1].pnum;
    file_read_page(neighbor_pnum, &neighbor);

    /* Coalescence. */

    if (neighbor.node_page.header.num_keys + current.node_page.header.num_keys < capacity)
        return coalesce_nodes(root_pnum, current_pnum, &current, neighbor_pnum, &neighbor, neighbor_index, k_prime);

    /* Redistribution. */

    else
        redistribute_nodes(root_pnum, current_pnum, &current, neighbor_pnum, &neighbor, neighbor_index, k_prime, k_prime_index);
        return root_pnum;
}
//
//
//
///* Master deletion function.
// */
int delete(int64_t key){
    char temp_val[VALUE_SIZE];
    
    if( find(key,temp_val) == FAILURE){
    	printf("ERR: Key %ld not found!\n", key);
    	return FAILURE;
    }
    
    file_read_header();
    pagenum_t key_leaf = find_leaf(main_header.header_page.root_page_no, key);
    
    if(key_leaf == 0){
    	printf("ERR: Tree is empty!\n");
    	return FAILURE;
    }

    //get new root page number after deleting
    main_header.header_page.root_page_no = delete_entry(main_header.header_page.root_page_no, key_leaf, key);
    file_write_header();
    
    return SUCCESS;
}
