#ifndef __RUN_LINES_HEADER__
#define __RUN_LINES_HEADER__

#include "common.h"

typedef struct
{
	int line;
	int reps;
} RunLine;

typedef struct 
{
	int capacity;
	int count;
	RunLine* lines;
} RunLineArray;

void linearr_init(RunLineArray* array);
void linearr_write(RunLineArray* array, int line);
int linearr_get_line(RunLineArray* array, int offset);
void linearr_free(RunLineArray* array);

#endif
