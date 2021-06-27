#pragma once

#include "common.h"
#include "bytecode.h"
#include "kscan.h"

typedef void (*ParseFn)();

typedef struct
{
	Token current;
	Token previous;
	bool hadError;
	bool isPanic;
} Parser;

typedef enum 
{
	PREC_NONE,
	PREC_ASSIGNMENT,  // =
	PREC_OR,          // or
	PREC_AND,         // and
	PREC_EQUALITY,    // is , is not
	PREC_COMPARISON,  // < > <= >=
	PREC_TERM,        // + -
	PREC_FACTOR,      // * /
	PREC_UNARY,       // not -
	PREC_CALL,        // . ()
	PREC_PRIMARY
} Precedence;

typedef struct {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

void grouping();
void unary();
void binary();
void expression();
void statement();
void declaration();
void number();
void literal();
void string();

Parser parser;
ByteCode* compilingChunk;

bool compile(const char* source, ByteCode* chunk);