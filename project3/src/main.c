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

    init_db(BUFFER_SIZE);
    printf("> ");
    while (scanf("%c", &instruction) != EOF) {
        switch (instruction) {
        case 't':
               printf("======Current table_id list:=======\n");
        	for(int i=0 ; i<10 ; i++){
        	    printf("table_id:%d\nfilename:%s\n",tid_list[i].table_id,tid_list[i].pathname);
        	    printf("\n");
        	}
        	break;
        case 'o':
            scanf("%s",filename);
            tid = open_table(filename);
            break;
        case 'd':
            scanf("%d %ld",&tid, &key);
            flag = db_delete(tid, key);
            if(flag == SUCCESS) printf("Key %ld successfully deleted!\n", key);
            else printf("ERR: Deletion failed.\n");
            break;
        case 'i':
            scanf("%d %ld %s",&tid, &key,value);
            flag = db_insert(tid, key, value);
            if(flag == SUCCESS) printf("Key %ld (value: %s) successfully inserted!\n", key, value);
            else printf("ERR: Insertion failed.\n");
            break;
        case 'f':
            scanf("%d %ld",&tid, &key);
            flag = db_find(tid,key,value);
            if(flag == SUCCESS) printf("Key: %ld\nValue: %s\n", key,value);
            else printf("ERR: Finding failed.\n");
            break;
        case 'c':
            scanf("%d",&tid);
            close_table(tid);
            break;    
        case 'q':
            while (getchar() != (int)'\n');
            shutdown_db();
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
