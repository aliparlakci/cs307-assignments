#ifndef HEAP_MANAGER
#define HEAP_MANAGER

#include <stdio.h>
#include <list>
#include <mutex>

struct Segment
{
    int ID;
    int size;
    int index;
};

class HeapManager
{
public:
    int initHeap(int size)
    {
        pthread_mutex_init(&lock, NULL);
        pthread_mutex_lock(&lock);
        memory.clear();
        memory.push_back(Segment{-1, size, 0});

        for (auto iter = memory.begin(); iter != memory.end();)
        {
            printf("[%d][%d][%d]", iter->ID, iter->size, iter->index);
            if (++iter != memory.end())
                printf("---");
        }
        printf("\n");

        pthread_mutex_unlock(&lock);
        return 1;
    };
    int myMalloc(int ID, int size)
    {
        int index = -1;
        pthread_mutex_lock(&lock);
        for (auto item = memory.begin(); item != memory.end() && index == -1; item++)
        {
            if (item->ID == -1 && item->size >= size)
            {
                index = item->index;
                item->ID = ID;
                int free_space = item->size - size;
                item->size = size;
                if (free_space)
                {
                    memory.insert(++item, Segment{-1, free_space, item->index + size});
                    item--;
                }

                break;
            }
        }
        if (index != -1)
            printf("Allocated for thread %d \n", ID);
        else
            printf("Can not allocate, requested size %d for thread %d is bigger than remaining size\n", size, ID);

        for (auto iter = memory.begin(); iter != memory.end();)
        {
            printf("[%d][%d][%d]", iter->ID, iter->size, iter->index);
            if (++iter != memory.end())
                printf("---");
        }
        printf("\n");

        pthread_mutex_unlock(&lock);
        return index;
    };
    int myFree(int ID, int index)
    {
        int result = -1;
        pthread_mutex_lock(&lock);
        int i = 0;
        for (auto iter = memory.begin(); iter != memory.end(); iter++)
        {
            if (iter->index == index)
            {
                if (iter->ID != ID)
                {
                    pthread_mutex_unlock(&lock);
                    return -1;
                }

                iter->ID = -1;

                result = merge(i);
                break;
            }
            i++;
        }
        if (result != -1)
            printf("Freed for thread %d\n", ID);

        for (auto iter = memory.begin(); iter != memory.end();)
        {
            printf("[%d][%d][%d]", iter->ID, iter->size, iter->index);
            if (++iter != memory.end())
                printf("---");
        }
        printf("\n");

        pthread_mutex_unlock(&lock);
        return result;
    };
    void print()
    {
        pthread_mutex_lock(&lock);
        for (auto iter = memory.begin(); iter != memory.end();)
        {
            printf("[%d][%d][%d]", iter->ID, iter->size, iter->index);
            if (++iter != memory.end())
                printf("---");
        }
        printf("\n");
        pthread_mutex_unlock(&lock);
    };

private:
    std::list<Segment> memory;
    pthread_mutex_t lock;
    int merge(int i)
    {
        auto position = memory.begin();
        for (int x = 0; x < i; x++, position++)
            ;

        auto ptr = memory.begin();
        for (int x = 0; x < i; x++, ptr++)
            ;

        if (ptr->ID != -1)
            return -1;

        if (++ptr != memory.end())
        {
            if (ptr->ID == -1)
            {
                position->size += ptr->size;
                memory.erase(ptr);
                ptr--;
            }
        }

        ptr = memory.begin();
        for (int x = 0; x < i; x++, ptr++)
            ;

        if (ptr != memory.begin())
        {
            if ((--ptr)->ID == -1)
            {
                ptr->size += position->size;
                memory.erase(position);
                ptr++;
            }
        }

        return 1;
    };
};

#endif