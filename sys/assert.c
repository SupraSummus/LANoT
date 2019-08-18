#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "io.h"

/**
 * it's like standard newlib's assert but first it swicthes IO drivers to synchronous mode
 */

void __assert_func (
	const char *file,
	int line,
	const char *func,
	const char *failedexpr
) {
	io_use_synchronous_mode(STDERR_FILENO);
	fprintf(stderr,
		"assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
		failedexpr, file, line,
		func ? ", function: " : "",
		func ? func : ""
	);
	abort();
}
