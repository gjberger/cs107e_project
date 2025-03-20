#include <stdint.h>
#include "random.h"
#include "timer.h"

uint64_t get_seed(void) {
	return (uint64_t)timer_get_ticks();
}

// middle-square Weyl Sequence randon number generator
uint32_t randGen(void) {
	static uint64_t x = 0;
	static uint64_t w = 0;
	static uint64_t s = 0xb5ad4eceda1ce2a9;

	if (x == 0) {
		x = get_seed();
	}

	x *= x;
	x += (w += s);
	return (x = (x >> 32) | (x << 32));
}

// return random number 0 - (range - 1)
uint32_t rand(int range) {
	return (randGen() % range);
}
