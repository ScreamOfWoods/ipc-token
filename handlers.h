#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void handle_error(int err_code)
{
	fprintf(stderr, "Got error %s\n", strerror(err_code));
	exit(EXIT_FAILURE);
}

#endif //HANDLERS_H
