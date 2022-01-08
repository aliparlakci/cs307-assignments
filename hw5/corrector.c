#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct node
{
    char *name;
    char *surname;
    char *sex;
    struct node *next;
} list_node_t;

void read_database(list_node_t *db);
void correct_file(char * location, list_node_t *db);
list_node_t *list_init();
void list_append(list_node_t *head, char *sex, char *name, char *surname);
char *list_find_sex(list_node_t *head, char *name);
char *list_find_surname(list_node_t *head, char *name);
void list_print(list_node_t *head);
void list_destroy(list_node_t *head);

int main(int argc, char argv[])
{
    list_node_t *db = list_init();

    read_database(db);
    correct_file("example.txt", db);

    list_destroy(db);
    return 0;
}

void read_database(list_node_t *db)
{
    FILE * file = fopen("database.txt", "r+");
    
    char *sex, *name, *surname;
    sex = (char *) malloc(sizeof(char) * 256);
    name = (char *) malloc(sizeof(char) * 256);
    surname = (char *) malloc(sizeof(char) * 256);

    while (fscanf(file, "%s %s %s\n", sex, name, surname) != EOF)
    {
        list_append(db, sex, name, surname);
    }
}

void correct_file(char * location, list_node_t *db)
{
    FILE * fptr = fopen(location, "r+");

    long cursor = ftell(fptr);
    char buffer[256];
    while(fscanf(fptr, "%s", buffer) != EOF)
    {
        long name_cursor, surname_cursor;
        char name[256], surname[256];
        if (strcmp(buffer, "Ms.") || strcmp(buffer, "Mr."))
        {
            name_cursor = ftell(fptr);
            fscanf(fptr, "%s", name);
            
            surname_cursor = ftell(fptr);
            fscanf(fptr, "%s", surname);

            fseek(fptr, cursor, SEEK_SET);
            fputs(list_find_sex(db, name), fptr);

            fseek(fptr, surname_cursor, SEEK_SET);
            fputs(list_find_surname(db, name), fptr);
        }

        cursor = ftell(fptr);
    }

    fclose(fptr);
}

// list functions begin

list_node_t *list_init()
{
    list_node_t *head = (list_node_t *)malloc(sizeof(list_node_t));
    return head;
}

void list_append(list_node_t *head, char *sex, char *name, char *surname)
{
    list_node_t *ptr = head;
    while (ptr->next)
        ptr = ptr->next;

    ptr->next = (list_node_t *)malloc(sizeof(list_node_t));
    assert(ptr->next);
    ptr->next->sex = strdup(sex);
    ptr->next->name = strdup(name);
    ptr->next->surname = strdup(surname);
}

char *list_find_sex(list_node_t *head, char *name)
{
    list_node_t *ptr = head->next;

    while (ptr)
    {
        if (strcmp(name, ptr->name))
        {
            return ptr->sex;
        }
    }

    return 0;
}

char *list_find_surname(list_node_t *head, char *name)
{
    list_node_t *ptr = head->next;

    while (ptr)
    {
        if (strcmp(name, ptr->name))
        {
            return ptr->surname;
        }
    }

    return 0;
}

void list_print(list_node_t *head)
{
    list_node_t *ptr = head->next;

    while (ptr)
    {
        printf("%s %s %s\n", ptr->sex, ptr->name, ptr->surname);

        ptr = ptr->next;
    }
}

void list_destroy(list_node_t *head)
{
    list_node_t *ptr = head;
    list_node_t *next = head->next;

    while (next)
    {
        free(ptr->sex);
        free(ptr->name);
        free(ptr->surname);
        free(ptr);
        ptr = next;
        next = ptr->next;
    }
    free(ptr);
}

// list functions end