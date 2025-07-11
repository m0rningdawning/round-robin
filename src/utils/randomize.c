#include "randomize.h"

#include <sys/random.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t s_random_int(uint32_t min, uint32_t max) {
  uint32_t bound = max - min + 1;

  uint32_t r, limit = UINT32_MAX - (UINT32_MAX % bound);

  do {
    if (getrandom(&r, sizeof(r), 0) != sizeof(r)) {
      perror("Error in the function getrandom()");
      exit(EXIT_FAILURE);
    }
  } while (r >= limit);

  return r % bound + min;
}
