#ifndef HISTORY_H
#define HISTORY_H

#include <stddef.h>
#include <stdbool.h>

#define ORB_CAPACITY 16

typedef struct {
    Message buf[ORB_CAPACITY];
    size_t  head;
    size_t  tail;
    bool    full;
} OverwritingRingBuffer;

void ORB_init(OverwritingRingBuffer *rb);
void ORB_push(OverwritingRingBuffer *rb, Message const *item);
bool ORB_pop(OverwritingRingBuffer *rb, Message *out);
void ORB_clear(OverwritingRingBuffer *rb);
bool ORB_isEmpty(OverwritingRingBuffer *rb);
bool ORB_isFull(OverwritingRingBuffer *rb);
size_t ORB_size(OverwritingRingBuffer *rb);

#endif // HISTORY_H