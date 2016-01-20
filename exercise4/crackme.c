/*
 * Copyright (C) 2016 Tamas K Lengyel
 * ACADEMIC PUBLIC LICENSE
 * For details please read the LICENSE file.
 */

/*
 * gcc crackme.c -o crackme
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void gen_random_string(char *s, const int len) {
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    int i = 0;
    for (; i < len; ++i) {
        s[i] = alphanum[random() % (sizeof(alphanum) - 1)];
    }
}

// Assumes 0 <= max <= RAND_MAX
// Returns in the half-open interval [0, max]
static long random_number(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}

int main(int argc, char **argv) {

	char *randomness = malloc(4096);
	srand(time(NULL));
	gen_random_string(randomness, 4096);

	while (1) {

	long start = random_number(4088);
	//printf("Magic word is %.8s\n", &randomness[start]);

		while(1) {

			puts("Enter the magic string:");
			char buf[100];
        		bzero(buf, 100);
			fgets(buf, 100, stdin);

        		if(!strncmp(buf, &randomness[start], 8)) {
				puts("Found the magic string!\n");
				break;
        		}
		}
  	}

	free(randomness);
}
