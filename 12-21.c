#include "csapp.h"

int readcount, writecount;
sem_t mutex1, mutex2, mutex3, w, r;// init value 1
// mutex1 用来保证 readcount 的排他操作
// mutex2 用来保证 writecount 的排他操作
// w 用来保证有一串读者操作时，写者必须等待到最后一个读者离开；有写者操作时，读者必须等待
// r 用来保证有写者到来时，所有后续的读者必须要等待
// mutex3 用来保证读者的 P(&r) 到 V(&r) 的操作是排他的，就不会有读者和写者一起等待 V(&r)，保证了写者的优先权。

void *reader(void *v)
{
    P(&mutex3);
    P(&r);
    P(&mutex1);
    readcount++;
    if (readcount == 1) P(&w);
    V(&mutex1);
    V(&r);// 可不可以直接放在 P(&r) 后，不可以。因为读者写者对 w 的竞争是互斥的，当写者成功执行 P(&r) 时，需要保证读者无法执行到 P(&w)，确保写者更高的优先级。
    V(&mutex3);

    // reading is done

    P(&mutex1);
    readcount--;
    if (readcount == 0) V(&w);
    V(&mutex1);
}

void *writer(void *v)
{
    P(&mutex2);
    writecount++;
    if (writecount == 1) P(&r);
    V(&mutex2);
    P(&w);

    // writing is performed

    V(&w);
    P(&mutex2);
    writecount--;
    if (writecount == 0) V(&r);
    V(&mutex2);
}