#ifndef HEAP_MANAGER
#define HEAP_MANAGER

#include <list>

struct Segment
{
    int ID;
    int size;
    int index;
};

class HeapManager
{
public:
    int initHeap(int size);
    int myMalloc(int ID, int size);
    int myFree(int ID, int index);
    void print();
private:
    std::list<Segment> memory;
    pthread_mutex_t lock;
    int merge(int i);
};

#endif