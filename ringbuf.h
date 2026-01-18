#ifndef RINGBUF_H
#define RINGBUF_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_CAP 10

struct RingBuf ringbuf_alloc(int item_size);
void ringbuf_push(struct RingBuf *rb, void *item);
bool ringbuf_is_full(struct RingBuf *rb);
bool ringbuf_is_empty(struct RingBuf *rb);
void ringbuf_resize(struct RingBuf *rb, int new_size);
void ringbuf_pop(struct RingBuf *rb, void *popped_event);
void ringubf_peek(struct RingBuf *rb, void *item);
void ringbuf_free(struct RingBuf *rb);
void ringbuf_dump(struct RingBuf *rb);

struct RingBuf {
  void **items;
  int out;
  int in;
  int size;
  int capacity;
  int item_size;
};

#endif // RINGBUF_H

#ifdef RINGBUF_IMPLEMENTATION

struct RingBuf ringbuf_alloc(int item_size) {
  assert(item_size > 0);

  struct RingBuf rb;
  rb.items = (void **)malloc(sizeof(void *) * INIT_CAP);
  rb.out = 0;
  rb.in = 0;
  rb.size = 0;
  rb.capacity = INIT_CAP;
  rb.item_size = item_size;

  return rb;
}

void ringbuf_push(struct RingBuf *rb, void *item) {
  assert(rb != NULL);
  assert(item != NULL);

  if (ringbuf_is_full(rb)) {
    ringbuf_resize(rb, rb->capacity * 1.5);
  }

  void *new_item = malloc(rb->item_size);
  memcpy(new_item, item, rb->item_size);

  rb->items[rb->in] = new_item;
  rb->in = (rb->in + 1) % (rb->capacity);
  rb->size++;
}

inline bool ringbuf_is_full(struct RingBuf *rb) {
  return rb->size == rb->capacity;
}

inline bool ringbuf_is_empty(struct RingBuf *rb) { return rb->size == 0; }

void ringbuf_resize(struct RingBuf *rb, int new_size) {
  assert(new_size > rb->capacity);

  void **new_buf = (void **)malloc(sizeof(void *) * new_size);

  for (int i = 0; i < rb->size; i++) {
    int ring_buf_idx = (rb->out + i) % rb->capacity;
    new_buf[i] = rb->items[ring_buf_idx];
  }

  free(rb->items);
  rb->items = new_buf;
  rb->out = 0;
  rb->in = rb->size;
  rb->capacity = new_size;
}

void ringbuf_pop(struct RingBuf *rb, void *popped_event) {
  assert(rb != NULL);
  assert(!ringbuf_is_empty(rb));

  if (popped_event != NULL) {
    memcpy(popped_event, rb->items[rb->out], rb->item_size);
  }

  free(rb->items[rb->out]);
  rb->items[rb->out] = NULL;
  rb->out = (rb->out + 1) % rb->capacity;
  rb->size--;
}

void ringubf_peek(struct RingBuf *rb, void *item) {
  assert(!ringbuf_is_empty(rb));

  memcpy(item, rb->items[rb->out], rb->item_size);
}

void ringbuf_free(struct RingBuf *rb) {
  for (int i = 0; i < rb->size; i++) {
    int rb_idx = (rb->out + i) % rb->capacity;
    free(rb->items[rb_idx]);
  }

  free(rb->items);
  rb->items = NULL;
  rb->capacity = rb->size = rb->out = rb->in = 0;
}

void ringbuf_dump(struct RingBuf *rb) {
  fprintf(stderr, "--------------------RingBuf Dump--------------------");

  for (int i = 0; i < rb->capacity; i++) {
    if (i % 4 == 0)
      fprintf(stderr, "\n");

    int rb_idx = (rb->out + i) % rb->capacity;
    fprintf(stderr, "%p ", rb->items[rb_idx]);
  }
  fprintf(stderr, "\n-----------------------------------------------------\n");
}
#endif // RINGBUF_IMPLEMENTATION
