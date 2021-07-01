#pragma once

#include "common.h"
#include "bytecode.h"
#include "kscan.h"
#include "kparser.h"

typedef void (*ParseFn)(KCompiler* compiler);

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

typedef struct {
	Parser parser;
	ByteCode* bytec;
} KCompiler;

bool kcom_compile(const char* source, ByteCode* chunk);

inline ParseRule* get_rule(ParseRule* parseRules, TokenType type)
{
	return &parseRules[type];
}