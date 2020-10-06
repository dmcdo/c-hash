#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "../chash.h"

int cmpfunc(const void *a, const void *b, size_t size)
{
    return (*(int *)a - *(int *)b);
}

int main(int argc, char const *argv[])
{
    // Create a new hashtable that maps a string key to an integer value
    struct chash *hashtable = chash_create(CHASH_TYPE_STRING,
                                           CHASH_STRING_HASH,
                                           CHASH_STRING_CMP,
                                           sizeof(int));

    int do_loop = true;
    while (do_loop)
    {
        char choice;
        char key[21];
        int val;
        int *p_val;

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

            chash_insert(hashtable, key, &val);
            printf("Key-value pair inserted!\n");
            break;

        case 'r':
            printf("Enter key (20 characters max please): ");
            scanf("%s", key);

            chash_remove(hashtable, key);
            printf("Key-value pair removed!\n");
            break;

        case 'f':
            printf("Enter key (20 characters max please): ");
            scanf("%s", key);

            p_val = (int *)chash_find(hashtable, key);

            if (p_val == NULL)
            {
                printf("Definition for \"%s\" not found.\n", key);
            }
            else
            {
                printf("\"%s\" => %d\n", key, *p_val);
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

    // We're done using the hashtable, so we must delete it
    chash_destroy(&hashtable);

    puts("Goodbye!");
    return 0;
}
