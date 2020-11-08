#include "dbapi.h"

// MAIN

int main( int argc, char ** argv ) {

    //char * input_file;
    //FILE * fp;
    //node * root;
    //int input, range2;
    int64_t key;
    char instruction;
    char value[VALUE_SIZE];
    int flag;
    //char license_part;

    //root = NULL;
    //verbose_output = false;

    //if (argc > 1) {
    //    order = atoi(argv[1]);
    //    if (order < MIN_ORDER || order > MAX_ORDER) {
     //       fprintf(stderr, "Invalid order: %d .\n\n", order);
    //        usage_3();
    //        exit(EXIT_FAILURE);
    //    }
   // }

    //license_notice();
    //usage_1();  
    //usage_2();

    //if (argc > 2) {
    //    input_file = argv[2];
    //    fp = fopen(input_file, "r");
    //    if (fp == NULL) {
    //        perror("Failure  open input file.");
    //        exit(EXIT_FAILURE);
    //    }
    //    while (!feof(fp)) {
    //        fscanf(fp, "%d\n", &input);
    //        root = insert(root, input, input);
    //    }
    //    fclose(fp);
    //    print_tree(root);
   // }
   
   int unique_id = open_table("sample_10000.db");

    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 'd':
            scanf("%ld", &key);
            flag = db_delete(key);
            if(flag == SUCCESS) printf("Key %ld successfully deleted!\n", key);
            else printf("ERR: Deletion failed.\n");
            break;
        case 'i':
            scanf("%ld %s", &key,value);
            flag = db_insert(key, value);
            if(flag == SUCCESS) printf("Key %ld (value: %s) successfully inserted!\n", key, value);
            else printf("ERR: Insertion failed.\n");
            break;
        case 'f':
            scanf("%ld",&key);
            flag = db_find(key,value);
            if(flag == SUCCESS) printf("Key: %ld\nValue: %s\n", key,value);
            else printf("ERR: Finding failed.\n");
            break;
        case 'q':
            while (getchar() != (int)'\n');
            return EXIT_SUCCESS;
            break;
        default:
            break;
        }
        while (getchar() != (int)'\n');
        printf("> ");
    }
    printf("\n");

    return SUCCESS;
}
