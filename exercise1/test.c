/*
 * Copyright (C) 2016 Tamas K Lengyel
 * ACADEMIC PUBLIC LICENSE
 * For details please read the LICENSE file.
 */
#include <stdio.h>
#include <stdlib.h>

void main(void) {

	int *test = malloc(sizeof(int));
	*test = 1;

	do {
		//printf("My PID is %i. The test pointer is at 0x%lx on the stack, it points to 0x%lx on the heap where the value is %i.\n", getpid(), &test, test, *test);
		printf("My PID is %i. The test pointer is at 0x%lx on the stack, it points to ??? on the heap where the value is %i.\n", getpid(), &test, *test);
		sleep(10);
	} while (1);

	free(test);
}
