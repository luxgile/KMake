#pragma once

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

void initLinesArray(RunLineArray* array);
void writeLinesArray(RunLineArray* array, int line);
int getLine(RunLineArray* array, int offset);
void freeLinesArray(RunLineArray* array);
