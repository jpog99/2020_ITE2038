# Disk Based B+ Tree
## 1. Introduction to B+ Tree
B+ Tree is a tree-type data structure that provide a very optimized operations of insertion, deletion and find. This tree uses a key record pair to store the data where key decides where the value is located in the tree and the record itself holds the data the user working on (In this project, our records hold integers as data). An example of a B+ Tree is as below:

![image of bpt](https://github.com/jpog99/2020_ITE2038/blob/master/project2/image/1.PNG) 

Root node: Node at the top of the tree

Internal nodes: Nodes at the middle of the tree (i.e neither at top or bottom level)

Leaf nodes: Nodes at the bottom layer of the tree

*Notice that all leaf nodes are linked in linked list. This means we can execute the find operation by sequential order too.*

B+ tree has a special constrain which is called the order of tree. This order will determine the maximum number of records in any nodes. For example, given a B+ Tree with a order of 3, then every nodes of the tree cannot hold 3 or more records. If this policy is violated during any operation, the tree will need to rebalance itself which will be explained in the next section.

## 2. ```bpt.c``` Operations Analysis
In this project, we were given the source code of B+ Tree In-Memory type. We will first analyze the call path of 2 operations, insertion and deletion.

### 1. Insertion
Insertion in B+ Tree follows a few steps.

1. Find the position in the tree and insert the new input.

2. If the leaf node violates the constrain (order) of the tree, copy the middle value of the leaf into the internal node and split that leaf node.

3. If the internal node still violates the degree of the tree, copy the middle value of the node into the next internal node and split the node before into half.

**Example**

![image](https://github.com/jpog99/2020_ITE2038/blob/master/project2/image/2.PNG) *Assuming the degree of tree is 3, we will insert a new input 26.*
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project2/image/3.PNG) *Now the leaf node violates the balancing policy and the node is splitted by half.*
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project2/image/4.PNG) *Middle value is copied to internal node and again violating the balancing policy.*
![image](https://github.com/jpog99/2020_ITE2038/blob/master/project2/image/5.PNG) *Repeat the operation until the tree is balanced.*

Now we will look at how the insertion operation is executed in `bpt.c` using function `node * insert()`. This function takes in 3 parameters and assign new `record*` and `node*`. This function also handles 4 different cases of how the insertion occurs.

  **Case 1: Ignoring new input that already exist in tree**
```c
 if (find(root, key, false) != NULL)
        return root;
```
  **Case 2: No tree created thus creating a new tree**
```c
if (root == NULL) 
        return start_new_tree(key, pointer);
```
  **Case 3: The insertion doesn't violate the tree balance.**
```c
leaf = find_leaf(root, key, false);
if (leaf->num_keys < order - 1) {
        leaf = insert_into_leaf(leaf, key, pointer);
        return root;
    }
```
  **Case 4: The insertion violates the balance policy and node is splitted**
```c
return insert_into_leaf_after_splitting(root, leaf, key, pointer);
```
### 2. Deletion
In B+ Tree, every leaf nodes must have more than order-1 records. If a leaf node only have 1 record, the node will have to merge with the neighboring leaf node. In `bpt.c` the master function for deletion which is `node * delete()` call the next function `node * delete_entry` as below.
```c
node * delete(node * root, int key) {

    node * key_leaf;
    record * key_record;

    key_record = find(root, key, false);
    key_leaf = find_leaf(root, key, false);
    if (key_record != NULL && key_leaf != NULL) {
        root = delete_entry(root, key_leaf, key, key_record);
        free(key_record);
    }
    return root;
}
```
In this case, the function `delete_entry()` will be handling different cases of deletion in  this implementation.

  **Case 1: Deleting value from root, where the copy in the internal node will be deleted too.**
```c
if (n == root) 
        return adjust_root(root);
```
  **Case 2: The number of record in leaf node isn't less than minimum**
```c
if (n->num_keys >= min_keys)
        return root;
```
The minimum value is determined by 
```c
min_keys = n->is_leaf ? cut(order - 1) : cut(order) - 1;
```
  **Case 3: Number of record falls below, but mergeable with neighbor leaf (Coalescence) or not (Redistribution)**
```c
if (neighbor->num_keys + n->num_keys < capacity)
        return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);
else
        return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
```


## 2.  Flow of The Structure Modifications
In insertion and deletion operation, the functions handles and rebalance the tree every time the balancing policy is violated. This time we will look deeper into these operations that modify our whole tree structure.

### 1. Split
As mentioned before, if the insertion operation causes the leaf node to overflow, the middle value is copied to its parent node and the leaf node before is split in half. To show the call flow, we will start from the `insert()` function case 4.

1. Leaf node is overflowed, thus calling `insert_into_leaf_after_splitting()` function.
```c
return insert_into_leaf_after_splitting(root, leaf, key, pointer);
```
2. Now the leaf is split, this function calls `insert_into_parent()` function to copy middle value to parent node.
```c
node * insert_into_leaf_after_splitting(node * root, node * leaf, int key, record * pointer) {
        
        //...
        split = cut(order-1);     //calling cut() to split
        //...
        new_leaf->parent = leaf->parent;
        new_key = new_leaf->keys[0];

        return insert_into_parent(root, leaf, new_key, new_leaf); //copying mid value to parent
```
3. `insert_into_parent()` handles few different cases and calls `insert_into_nodes_after_splitting()` if parent node is overflow too.
```c
node * insert_into_parent(node * root, node * left, int key, node * right) {
    int left_index;
    node * parent;
    parent = left->parent;

    //case 1: the root is overflowing
    if (parent == NULL)
        return insert_into_new_root(left, key, right);

    left_index = get_left_index(parent, left);


    //case 2: the parent didn't overflow
    if (parent->num_keys < order - 1)
        return insert_into_node(root, parent, left_index, key, right);

    //case 3: the parent also overflow
    return insert_into_node_after_splitting(root, parent, left_index, key, right);
}
```
4. Finally, `insert_into_nodes_after_splitting()` calls `insert_into_parent()` again and again until case 2 from above is achieved. This means the balancing policy of the tree is now preserved. `insert_into_node()` doesn't call any other function anymore.
```c 
node * insert_into_node_after_splitting(node * root, node * old_node, int left_index, 
        int key, node * right) {
     
      //.....
      return insert_into_parent(root, old_node, k_prime, new_node);
```

### 2. Coalescence
As mentioned in deletion, if a leaf node is underflowed, the tree has to either coalescence or redistribute to keep the balancing policy. Coalescence occur if the *neighbor of underflowed node has extra space left*. Then, the record in the underflowed node will merge into that extra space in neighbor node.

In `bpt.c`, coalescence is executed by the function `coalesce_node()` which appends the median value `k_prime` from the underflowing node to the neighboring node and deletes that `k_prime` from parent node through `delete_entry()`.
```c
root = delete_entry(root, n->parent, k_prime, n);
free(n->keys);
free(n->pointers);
free(n); 
return root;
```

### 3. Redistribution
In contrast with coalescence, if a leaf node is underflowed after deletion and the neighboring leaf doesn't have any extra space left, then that neighboring leaf will have to distribute some of the records they're holding to the underflowed leaf. Plus, because the distributed record now is the leftmost record of the neighbor leaf, that record will be copied to the parent node, replacing the old leftmost record of the neighbor leaf.

In `bpt.c`, the fuction `redistribute_nodes()` has to deal with 2 cases. Case 1 is when the underflowed node has a neighbor on the left, so the record in the underflowed node has to move 1 space to the right (within the node) to create an empty space at the far right of the node so that the record at the far left of the neighboring leaf can enter.

Case 2 is where underflowed node is the leftmost leaf. This time, the underflowed node will just take the first record from neighboring leaf and append it to the rightmost of the underflowed node. In either cases, the underflowed node `n` will have +1 more record and the neighbor leaf will have -1 less record.

```c
n->num_keys++;
neighbor->num_keys--;
```
Finally, the index in the parent node is modified accordingly.

## 3. Naive Designs or Required Changes For Building On-Disk B+ Tree
As mentioned before, we are given In-Memory version of B+ Tree initially. This can be seen by how each nodes of the current implementation is structed in `bpt.h` file as below.
```c
typedef struct node {
    void ** pointers;
    int * keys;
    struct node * parent;
    bool is_leaf;
    int num_keys;
    struct node * next; // Used for queue.
} node;
```
This code shows that all keys and parent node every nodes are initialized using `int*` and `node*` type respectively. In addition, the nodes are linked by using `void**` type pointers. To achieve the On-Disk implementation, first we need to reconstruct all of the nodes according to the assignment specifications. From now on, we will **recognize node as pages.**

###  1. Pages reconstruction
1. Header page
```c
typedef uint64_t pagenum_t;

struct header_page{
    pagenum_t free_page_number;
    pagenum_t root_page_number;
    pagenum_t number_of_pages;
    char reserved[4072];
};
```
2. Free page
```c
struct free_page{
   pagenum_t next_free_page_number;
   char not_used[4088]; //using char type for reserve because char size is 1
};
```
3. Record (key/val pair)
```c
struct Record{
   int64_t key;
   char word[120];
};
```
4. Key/Page_num pair (for Internal pages)
```c
struct KeyPagePair{
   int64_t key;
   pagenum_t page_num
};
```
5. Leaf/Internal pages
```c
struct page_t{
   //page header of leaf and internal are same
   pagenum_t parent_pnum;
   int isLeaf;
   int num_of_keys;
   char reserved[104];
   
   //leftmost_child in case of internal page, otherwise right_neighbor.
   union{
        pagenum_t leftmost_child_pnum;
        pagenum_t right_neighbor_pnum;
    };

   //KeyPagePair in case of internal page, otherwise Record.
   union{
        KeyPagePair key_page_pair[248];
        Record record[31];
    };
};
```

### 2. File Manager API
As we are implementing On-Disk B+ Tree, we will build our implementation to be able to work on with other external files. The given file manager API will be used to handle the functions to open, read, update and write the database files.

1. `int open_table(char* pathname)`

We will need to open the file first by giving the file path as an argument for this function. If the file does not exist, new file will be created on that path. After that the header page of the file will be read thus saving the contents of the header page as pointers.

2. `pagenum_t file_alloc_page();`

This function will allocate a free page in our list corresponding to a page from the external file. Thus, `free_page_number` will be decremented and `number_of_pages` will be incremented in our header page. In case of no free page, a new page will be created. Finally, the free page is updated with the data from file and it will write changes to file. This function returns the number of free page left in our list.

3. `void file_free_page(pagenum_t pagenum);`

Based on the argument, the page corresponding to the `pagenum` will be freed and added to our free pages list.  After that, the va;ue of `free_page_number` and `number_of_pages` in header page will be updated.

4. `void file_read_page(pagenum_t pagenum, page_t* dest);`

This function will load all the contents from the file to the memory temporarily. This is essential to modify the contents of the external file.

5. `void file_write_page(pagenum_t pagenum, const page_t* src);`

Finally, each page is written into the file after operations (insert/delete). We will keep `src` argument as constant to prevent any problem related to write error during writing the file.  

### 3. Delayed Merge
In the initial code, if we implement the usual merge while working with a file, we will need to read and write multiple times every time merge from delete function completed. What we want to achieve is only write once for insert/delete operation. Using the initial implementation will causes heavy disk I/O.

To solve this problem, we will need to implement additional structure modification function which is **delayed merge** where merge will occur only when all keys in the page has been deleted, regardless of the tree order. This will reduce cost performance and increase the speed of operation. 
