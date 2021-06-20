#include "kcompiler.h"
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "kscan.h"
#include "kdebug.h"
#include "bytecode.h"

ParseRule parseRules[] = {
  [TOKEN_LEFT_PAREN] = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RIGHT_BRACE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS] = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS] = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH] = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR] = {NULL,     binary, PREC_FACTOR},
  [TOKEN_IS] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NOT] = {unary,     NULL,   PREC_NONE},
  [TOKEN_IS_NOT] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL] = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_GREATER] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_OR_EQUALS] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_OR_EQUALS] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IDENTIFIER] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUMBER] = {number,   NULL,   PREC_NONE},
  [TOKEN_AND] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE] = {literal,     NULL,   PREC_NONE},
  //[TOKEN_FOR]				= {NULL,     NULL,   PREC_NONE},
  [TOKEN_METHOD] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF] = {NULL,     NULL,   PREC_NONE},
  //[TOKEN_NIL]				= {NULL,     NULL,   PREC_NONE},
  [TOKEN_OR] = {NULL,     NULL,   PREC_NONE},
  //[TOKEN_PRINT]			= {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BASE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE] = {literal,     NULL,   PREC_NONE},
  //[TOKEN_VAR]				= {NULL,     NULL,   PREC_NONE},
  //[TOKEN_WHILE]			= {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF] = {NULL,     NULL,   PREC_NONE},
};

static ByteCode* currentChunk()
{
	return compilingChunk;
}

static ParseRule* getRule(TokenType type)
{
	return &parseRules[type];
}

static void errorAt(Token* token, const char* message)
{
	if (parser.isPanic) return;

	parser.isPanic = true;
	fprintf(stderr, "[line %d] Error", token->line);

	if (token->type == TOKEN_EOF) {
		fprintf(stderr, " at end");
	}
	else if (token->type == TOKEN_ERROR) {
		// Nothing.
	}
	else {
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}

	fprintf(stderr, ": %s\n", message);
	parser.hadError = true;
}

static void errorAtPrevious(const char* message)
{
	errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message)
{
	errorAt(&parser.current, message);
}

static void advanceToken()
{
	parser.previous = parser.current;

	while (true)
	{
		parser.current = scanToken();
		if (parser.current.type != TOKEN_ERROR) break;

		errorAtCurrent(parser.current.start);
	}
}

static void parsePrecedence(Precedence precedence)
{
	advanceToken();
	ParseFn prefixRule = getRule(parser.previous.type)->prefix;
	if (prefixRule == NULL)
	{
		errorAtPrevious("Expect expression.");
		return;
	}

	prefixRule();

	while (precedence <= getRule(parser.current.type)->precedence)
	{
		advanceToken();
		ParseFn infixRule = getRule(parser.previous.type)->infix;
		infixRule();
	}
}

static void consume(TokenType type, const char* message)
{
	if (parser.current.type == type)
	{
		advanceToken();
		return;
	}

	errorAtCurrent(message);
}

static void emitByte(uint8_t byte)
{
	ByteCode_WriteChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
	emitByte(byte1);
	emitByte(byte2);
	emitByte(byte3);
}

static void emitReturn()
{
	emitByte(OP_RETURN);
}

static void endCompiler()
{
	emitReturn();

#ifdef DEBUG_PRINT_CODE
	if (!parser.hadError)
	{
		disassembleChunk(currentChunk(), "Compiled Bytecode");
	}
#endif
}

static uint8_t makeConstant(double value)
{
	int constant = ByteCode_AddConstant(currentChunk(), TYPEID_DEC, &value);
	if (constant > UINT8_MAX)
	{
		errorAtPrevious("Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}

static void emitConstant(double value)
{
	emitBytes(OP_CONSTANT, TYPEID_DEC, makeConstant(value));
}

void literal()
{
	switch (parser.previous.type)
	{
		case TOKEN_TRUE: emitByte(OP_TRUE); break;
		case TOKEN_FALSE: emitByte(OP_FALSE); break;
	}
}


void number()
{
	double value = strtod(parser.previous.start, NULL);
	emitConstant(value);
}

void unary()
{
	TokenType operatorType = parser.previous.type;

	// Compile the operand.
	parsePrecedence(PREC_UNARY);

	// Emit the operator instruction.
	switch (operatorType)
	{
	case TOKEN_MINUS: emitByte(OP_NEGATE); break;
	case TOKEN_NOT: emitByte(OP_NOT); break;
	default: return;
	}
}

void binary()
{
	TokenType operatorType = parser.previous.type;
	ParseRule* rule = getRule(operatorType);
	parsePrecedence((Precedence)(rule->precedence + 1));

	switch (operatorType)
	{
	case TOKEN_PLUS:          emitByte(OP_ADD); break;
	case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
	case TOKEN_STAR:          emitByte(OP_MULT); break;
	case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
	default: return;
	}
}

void expression()
{
	parsePrecedence(PREC_ASSIGNMENT);
}

void grouping()
{
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

bool compile(const char* source, ByteCode* chunk)
{
	compilingChunk = chunk;

	initScanner(source);

	parser.hadError = false;
	parser.isPanic = false;

	advanceToken();

	expression();

	consume(TOKEN_EOF, "Expect end of expression.");
	endCompiler();
	return !parser.hadError;
}
