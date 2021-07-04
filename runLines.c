#pragma once

#include "runLines.h"
#include <stdlib.h>
#include "common.h"
#include "kmem.h"

#define GET_LAST_LINE(array) (array->count == 0 ? NULL : &array->lines[array->count - 1])

void linearr_init(RunLineArray* array)
{
	array->capacity = 0;
	array->count = 0;
	array->lines = NULL;
}

void linearr_write(RunLineArray* array, int line)
{
	if (array->capacity < array->count + 1)
	{
		int oldCapacity = array->capacity;
		array->capacity = GROW_CAPACITY(oldCapacity);
		array->lines = GROW_ARRAY(RunLine, array->lines,
			oldCapacity, array->capacity);
	}

	RunLine* lastLine = GET_LAST_LINE(array);
	if (lastLine == NULL || lastLine->line != line)
	{
		RunLine runLine;
		runLine.line = line;
		runLine.reps = 1;

		array->lines[array->count] = runLine;
		array->count++;
	}
	else array->lines[array->count - 1].reps++;
}

void linearr_free(RunLineArray* array)
{
	FREE_ARRAY(RunLine, array->lines, array->capacity);
	linearr_init(array);
}

int linearr_get_line(RunLineArray* array, int offset)
{
	int count = 0;
	for (size_t i = 0; i < array->count; i++)
	{
		count += array->lines[i].reps;
		if (count >= offset)
			return array->lines[i].line;
	}
	
	return -1;
}
