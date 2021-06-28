#include "kcompiler.h"
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "kscan.h"
#include "kdebug.h"
#include "bytecode.h"
#include "commonTypes.h"

ParseRule parseRules[] = {
  [TOKEN_LEFT_PAREN] = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RIGHT_BRACE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SEMICOLON] = {NULL,     NULL,   PREC_NONE},

  [TOKEN_MINUS] = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS] = {NULL,     binary, PREC_TERM},
  [TOKEN_SLASH] = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR] = {NULL,     binary, PREC_FACTOR},

  //[TOKEN_IS] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_NOT] = {unary,     binary,   PREC_NONE},
  //[TOKEN_IS_NOT] = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_EQUALS] = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_GREATER] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_GREATER_OR_EQUALS] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_LESS_OR_EQUALS] = {NULL,     binary,   PREC_COMPARISON},

  [TOKEN_IDENTIFIER] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING] = {string,     NULL,   PREC_NONE},
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

static bool check(TokenType type)
{
	return parser.current.type == type;
}

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

static bool match(TokenType type)
{
	if (!check(type)) return false;
	advanceToken();
	return true;
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

static void emit2Bytes(uint8_t byte1, uint8_t byte2)
{
	emitByte(byte1);
	emitByte(byte2);
}

static void emit3Bytes(uint8_t byte1, uint8_t byte2, uint8_t byte3)
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

static uint8_t makeConstant(BYTE* byte, TYPE_ID id)
{
	int constant = ByteCode_AddConstant(currentChunk(), id, byte);
	if (constant > UINT8_MAX)
	{
		errorAtPrevious("Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}

static uint8_t makeConstantPointer(BYTE* byte, TYPE_ID id)
{
	int constant = ByteCode_AddConstantPointer(currentChunk(), id, byte);
	if (constant > UINT8_MAX)
	{
		errorAtPrevious("Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}

static void emitConstant(BYTE* bytes, TYPE_ID id)
{
	emit3Bytes(OP_CONSTANT, id, makeConstant(bytes, id));
}

static void emitConstantPointer(BYTE* bytes, TYPE_ID id)
{
	emit3Bytes(OP_CONSTANT, id, makeConstantPointer(bytes, id));
}

static void printStatement()
{
	expression();
	consume(TOKEN_SEMICOLON, "Expect ';' after value.");
	emitByte(OP_PRINT);
}

static void expressionStatement()
{
	expression();
	consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
	emitByte(OP_POP);
}

static void synchronize()
{
	parser.isPanic = false;

	while (parser.current.type != TOKEN_EOF)
	{
		if (parser.previous.type == TOKEN_SEMICOLON) return;
		switch (parser.current.type)
		{
		case TOKEN_CLASS:
		case TOKEN_IF:
		case TOKEN_PRINT:
		case TOKEN_RETURN:
			return;

		default:
			; // Do nothing.
		}

		advanceToken();
	}
}

static uint8_t identifierConstant(TYPE_ID type, Token* name)
{
	StringPointer* string = CopyString(name->start, name->length);
	return makeConstantPointer(string, type);
}

static void defineVariable(TYPE_ID type, uint8_t global) 
{
	emit3Bytes(OP_DEFINE_GLOBAL, global, type);
}

static uint8_t parseVariable(TYPE_ID type, const char* errorMessage) 
{
	consume(TOKEN_IDENTIFIER, errorMessage);
	return identifierConstant(type , &parser.previous);
}

static void varDeclaration(TYPE_ID type) 
{
	uint8_t global = parseVariable(type, "Expect variable name.");

	if (match(TOKEN_EQUALS))
	{
		expression();
	}

	consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

	defineVariable(type, global);
}

//static void namedVariable(Token name) 
//{
//	uint8_t arg = identifierConstant(&name);
//	emitBytes(OP_GET_GLOBAL, arg);
//}

void literal()
{
	switch (parser.previous.type)
	{
	case TOKEN_TRUE: emitByte(OP_TRUE); break;
	case TOKEN_FALSE: emitByte(OP_FALSE); break;
	}
}

void string()
{
	StringPointer* s = CopyString(parser.previous.start + 1, parser.previous.length - 2);
	emitConstantPointer(s, TYPEID_STRING);
}

void number()
{
	double value = strtod(parser.previous.start, NULL);
	emitConstant(&value, TYPEID_DEC);
}

//void variable() 
//{
//	namedVariable(parser.previous);
//}

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
	case TOKEN_PLUS:				emitByte(OP_ADD); break;
	case TOKEN_MINUS:				emitByte(OP_SUBTRACT); break;
	case TOKEN_STAR:				emitByte(OP_MULT); break;
	case TOKEN_SLASH:				emitByte(OP_DIVIDE); break;
	case TOKEN_EQUALS:				emitByte(OP_EQUALS); break;

		//case TOKEN_IS_NOT:				emit2Bytes(OP_EQUAL, OP_NOT); break;
		//case TOKEN_IS:					emitByte(OP_IS); break;	To define equality on types
	case TOKEN_GREATER:				emitByte(OP_GREAT); break;
	case TOKEN_GREATER_OR_EQUALS:	emit2Bytes(OP_LESS, OP_NOT); break;
	case TOKEN_LESS:				emitByte(OP_LESS); break;
	case TOKEN_LESS_OR_EQUALS:		emit2Bytes(OP_GREAT, OP_NOT); break;
	default: return;
	}
}

void expression()
{
	parsePrecedence(PREC_ASSIGNMENT);
}

void statement()
{
	if (match(TOKEN_PRINT))
	{
		printStatement();
	}
	else
	{
		expressionStatement();
	}
}

void declaration()
{
	if (match(TOKEN_IDENTIFIER))
	{
		TYPE_ID type = TypeTable_GetTypeId(parser.previous.start, parser.previous.length);
		if (type != TYPEID_VOID)
			varDeclaration(type);
		else
			errorAtCurrent("%s is not defined.", TypeTable_GetTypeInfo(type)->name);
	}
	else
	{
		statement();
	}

	if (parser.isPanic) synchronize();
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

	while (!match(TOKEN_EOF))
	{
		declaration();
	}

	consume(TOKEN_EOF, "Expect end of expression.");
	endCompiler();
	return !parser.hadError;
}
