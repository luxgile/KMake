#ifndef __KCOMPILER_HEADER__
#define __KCOMPILER_HEADER__

#include "common.h"
#include "bytecode.h"
#include "kscan.h"
#include "kparser.h"

typedef enum Precedence
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

typedef struct KCompiler {
	Parser parser;
	ByteCode* bytec;
} KCompiler;

typedef void (*ParseFn)(KCompiler* compiler);

typedef struct ParseRule {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;


bool kcom_compile(const char* source, ByteCode* chunk);

ParseRule* get_rule(ParseRule* parseRules, TokenType type);

#endif