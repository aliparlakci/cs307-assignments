#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>

typedef struct node
{
    char *name;
    char *surname;
    char *sex;
    struct node *next;
} list_node_t;

void traverse_directory(list_node_t *db);
void traverse_directory_rec(list_node_t *db, char *dir);
void read_database(list_node_t *db);
void correct_file(list_node_t *db, char * location);
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

    traverse_directory(db);

    list_destroy(db);
    return 0;
}

void traverse_directory(list_node_t *db)
{
    char path[256] = ".";

    DIR *dp = opendir(path);
    assert(dp != NULL);
    struct dirent *d;

    while ((d = readdir(dp)) != NULL)
    {
        char new_path[256];
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, d->d_name);
        if (d->d_type == 4 && strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0)
        {
            printf("D: %s\n", new_path);
            traverse_directory_rec(db, new_path);
        }
        else if (d->d_type == 8)
        {
            if ( strcmp(d->d_name, "database.txt") != 0 && strcmp(&d->d_name[strlen(d->d_name)-4], ".txt") == 0 )
            {
                printf("F: %s\n", new_path);      
                correct_file(db, new_path);
            }
        }
    }
}

void traverse_directory_rec(list_node_t *db, char *dir)
{
    char *path = strdup(dir);

    DIR *dp = opendir(path);
    assert(dp != NULL);
    struct dirent *d;

    while ((d = readdir(dp)) != NULL)
    {
        char new_path[256];
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, d->d_name);
        if (d->d_type == 4 && strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0)
        {
            printf("D: %s\n", new_path);
            traverse_directory_rec(db, new_path);
        }
        else if (d->d_type == 8)
        {
            if ( strcmp(&d->d_name[strlen(d->d_name)-4], ".txt") == 0 )
            {
                printf("F: %s\n", new_path);      
                correct_file(db, new_path);
            }
        }
    } 

    //free(path);  
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

void correct_file(list_node_t *db, char * location)
{
    FILE * fptr = fopen(location, "r+");

    long cursor = ftell(fptr);
    char buffer[256];
    while(fscanf(fptr, "%s", buffer) != EOF)
    {
        long name_cursor, surname_cursor;
        char name[256], surname[256];
        int is_woman = 0;

        if (strcmp(buffer, "Ms.") == 0 || strcmp(buffer, "Mr.") == 0)
        {
            name_cursor = ftell(fptr);
            fscanf(fptr, "%s", name);
            
            surname_cursor = ftell(fptr);
            fscanf(fptr, "%s", surname);

            is_woman = strcmp(list_find_sex(db, name), "f") == 0;

            fseek(fptr, name_cursor - 3, SEEK_SET);
            if (is_woman) fputs("Ms.", fptr);
            else fputs("Mr.", fptr);

            fseek(fptr, surname_cursor + 1, SEEK_SET);
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
        if (strcmp(name, ptr->name) == 0)
        {
            return ptr->sex;
        }
        ptr = ptr->next;
    }

    return 0;
}

char *list_find_surname(list_node_t *head, char *name)
{
    list_node_t *ptr = head->next;

    while (ptr)
    {
        if (strcmp(name, ptr->name) == 0)
        {
            return ptr->surname;
        }
        ptr = ptr->next;
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