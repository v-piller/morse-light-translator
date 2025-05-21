#include "history.h"

#include <stddef.h>
#include <stdbool.h>

// Initialise le buffer
void ORB_init(OverwritingRingBuffer *rb) {
    rb->head = rb->tail = 0;
    rb->full = false;
}

// Pousse un élément, écrase le plus ancien si plein
void ORB_push(OverwritingRingBuffer *rb, Message const *item) {
    rb->buf[rb->head] = *item;
    rb->head = (rb->head + 1) % ORB_CAPACITY;
    if (rb->full) {
        // on perd le plus ancien
        rb->tail = (rb->tail + 1) % ORB_CAPACITY;
    }
    rb->full = (rb->head == rb->tail);
}

// Récupère un élément, retourne false si vide
bool ORB_pop(OverwritingRingBuffer *rb, Message *out) {
    if (!rb->full && rb->head == rb->tail) {
        // vide
        return false;
    }
    *out = rb->buf[rb->tail];
    rb->full = false;
    rb->tail = (rb->tail + 1) % ORB_CAPACITY;
    return true;
}

// Vide totalement le buffer
void ORB_clear(OverwritingRingBuffer *rb) {
    rb->head = rb->tail;
    rb->full = false;
}

// Teste si vide
bool ORB_isEmpty(OverwritingRingBuffer *rb) {
    return (!rb->full && rb->head == rb->tail);
}

// Teste si plein
bool ORB_isFull(OverwritingRingBuffer *rb) {
    return rb->full;
}

// Nombre d’éléments actuellement stockés
size_t ORB_size(OverwritingRingBuffer *rb) {
    if (rb->full) {
        return ORB_CAPACITY;
    }
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    }
    return ORB_CAPACITY + rb->head - rb->tail;
}
