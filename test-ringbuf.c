#define RINGBUF_IMPLEMENTATION
#include "ringbuf.h"
#include <stdio.h>

void test_stress() {
  struct RingBuf rb = ringbuf_alloc(sizeof(int));

  int counter = 0;
  for (int i = 0; i < 100000; i++) {
    if (i % 3 == 0 && rb.size > 0) {
      ringbuf_pop(&rb, NULL);
    } else {
      ringbuf_push(&rb, &counter);
      counter++;
    }
  }

  printf("stress test done, size=%d cap=%d\n", rb.size, rb.capacity);

  ringbuf_free(&rb);
}

void test_trigger_resize() {
  struct RingBuf rb = ringbuf_alloc(sizeof(int));

  for (int i = 0; i < 13; i++) {
    ringbuf_push(&rb, &i);
  }

  printf("ringbuf dump\n");
  printf("out: %d, in:%d, size: %d, cap: %d\n", rb.out, rb.in, rb.size,
         rb.capacity);
  // ringbuf_dump(&rb);
  printf("\n");

  for (int i = 0; i < 5; i++) {
    ringbuf_pop(&rb, NULL);
  }

  for (int i = 0; i < 3; i++) {
    ringbuf_push(&rb, &i);
  }

  ringbuf_free(&rb);
}

int main() {
  test_trigger_resize();

  test_stress();

  return 0;
}
