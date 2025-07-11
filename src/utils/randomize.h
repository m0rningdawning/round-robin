#ifndef RANDOMIZE_H_
#define RANDOMIZE_H_

#include <stdint.h>

// This provides a random int in a certain range. Based on Linux's CSPRNG
uint32_t s_random_int(uint32_t min, uint32_t max);

#endif /* RANDOMIZE_H_ */
