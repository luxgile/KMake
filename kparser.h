#pragma once

#include "common.h"
#include "kcompiler.h"

//Helps the compiler to interpret tokens into op_codes
typedef struct
{
	Token current;
	Token previous;
	bool hadError;
	bool isPanic;
} Parser;

bool check_curr_type(Parser* parser, TokenType type);