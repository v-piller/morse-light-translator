#ifndef HISTORY_H
#define HISTORY_H
#include "time.h"
#include <stddef.h>
#include <stdbool.h>

#define ORB_CAPACITY 16

typedef struct {
    char text[32];
    bool receivedFromMorse;
    time_t timestamp; // Stores seconds since Jan 1, 1970
} Message;

typedef struct {
    Message buf[ORB_CAPACITY];
    size_t  head;
    size_t  tail;
    bool    full;
} OverwritingRingBuffer;

#ifdef __cplusplus
extern "C" {
#endif

void ORB_init(OverwritingRingBuffer *rb);
void ORB_push(OverwritingRingBuffer *rb, Message const *item);
bool ORB_pop(OverwritingRingBuffer *rb, Message *out);
void ORB_clear(OverwritingRingBuffer *rb);
bool ORB_isEmpty(OverwritingRingBuffer *rb);
bool ORB_isFull(OverwritingRingBuffer *rb);
size_t ORB_size(OverwritingRingBuffer *rb);

#ifdef __cplusplus
}
#endif

#endif // HISTORY_H