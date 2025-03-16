#include <stdint.h>
#include "random.h"

// middle-square Weyl Sequence randon number generator
uint32_t randGen(void) {
	static uint64_t x = 0;
	static uint64_t w = 0;
	static uint64_t s = 0xb5ad4eceda1ce2a9;

	x *= x;
	x += (w += s);
	return (x = (x >> 32) | (x << 32));
}

// return random number 0 - (range - 1)
uint32_t rand(int range) {
	return (randGen() % range);
}
