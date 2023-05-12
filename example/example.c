#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include "../chash.h"

int main(int argc, char const *argv[])
{
    // Create a new hashtable that maps a string key to an integer value
    CHASH *hashtable = chash_create(sizeof(char *),
                                           sizeof(int),
                                           chash_strhash,
                                           chash_strcmp,
                                           NULL);

    int do_loop = true;
    while (do_loop)
    {
        char choice;
        char key[21];
        int val;
        CHASH_KEY_VAL_PAIR *kv_pair;

        printf("What would you like to do? \n");
        printf("i: insert key-vaue pair, r: remove key-value pair\n");
        printf("f: find value by key, q: quit this program: ");

        scanf(" %c", &choice);

        switch (tolower(choice))
        {
        case 'i':
            printf("Enter key (20 characters max please): ");
            scanf("%s", key);

            printf("Enter the corresponding integer value: ");
            scanf("%d", &val);
            getchar(); // Absorb newline character

            char *k = (char *)malloc(21);        strcpy(k, key);
            int *v = (int *)malloc(sizeof(int)); *v = val;

            chash_insert(hashtable, k, v, NULL);
            printf("Key-value pair inserted!\n");
            break;

        case 'r':
            printf("Enter key (20 characters max please): ");
            scanf("%s", key);

            CHASH_KEY_VAL_PAIR *p = chash_remove(hashtable, key, NULL);

            free(p->key);
            free(p->val);

            printf("Key-value pair removed!\n");
            break;

        case 'f':
            printf("Enter key (20 characters max please): ");
            scanf("%s", key);

            kv_pair = chash_lookup(hashtable, key, NULL);

            if (kv_pair == NULL)
            {
                printf("Definition for \"%s\" not found.\n", key);
            }
            else
            {
                printf("\"%s\" => %d\n", (char *)kv_pair->key, *((int *)kv_pair->val));
            }
            break;

        case 'q':
            do_loop = false;
            break;

        default:
            printf("Invalid operation\n");
        }

        putchar('\n');
    }


    // Print all values in hashtable
    puts("");
    puts("Values left in hash");
    puts("-------------------");
    for (CHASH_ITERATOR i = chash_iterate_begin(hashtable); !i.end; chash_iterate_next(&i))
    {
        printf("\"%s\" => %d\n", (char *)i.key, *(int *)i.val);
    }

    // We're done using the hashtable, so we must delete it
    chash_delete(hashtable, true, true);

    puts("\nGoodbye!");
    return 0;
}
