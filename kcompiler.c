#include "kcompiler.h"
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "kscan.h"
#include "kdebug.h"
#include "bytecode.h"
#include "commonTypes.h"

void read_grouping(KCompiler* compiler);
void read_unary(KCompiler* compiler);
void read_binary(KCompiler* compiler);
void read_expression(KCompiler* compiler);
void read_statement(KCompiler* compiler);
void read_declaration(KCompiler* compiler);
void read_number(KCompiler* compiler);
void read_literal(KCompiler* compiler);
void read_string(KCompiler* compiler);

ParseRule parseRules[] = {
  [TOKEN_LEFT_PAREN] = {read_grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RIGHT_BRACE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SEMICOLON] = {NULL,     NULL,   PREC_NONE},

  [TOKEN_MINUS] = {read_unary,    read_binary, PREC_TERM},
  [TOKEN_PLUS] = {NULL,     read_binary, PREC_TERM},
  [TOKEN_SLASH] = {NULL,     read_binary, PREC_FACTOR},
  [TOKEN_STAR] = {NULL,     read_binary, PREC_FACTOR},

  //[TOKEN_IS] = {NULL,     binary,   PREC_COMPARISON},
  [TOKEN_NOT] = {read_unary,     read_binary,   PREC_NONE},
  //[TOKEN_IS_NOT] = {NULL,     binary,   PREC_EQUALITY},
  [TOKEN_EQUALS] = {NULL,     read_binary,   PREC_EQUALITY},
  [TOKEN_GREATER] = {NULL,     read_binary,   PREC_COMPARISON},
  [TOKEN_GREATER_OR_EQUALS] = {NULL,     read_binary,   PREC_COMPARISON},
  [TOKEN_LESS] = {NULL,     read_binary,   PREC_COMPARISON},
  [TOKEN_LESS_OR_EQUALS] = {NULL,     read_binary,   PREC_COMPARISON},

  [TOKEN_IDENTIFIER] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING] = {read_string,     NULL,   PREC_NONE},
  [TOKEN_NUMBER] = {read_number,   NULL,   PREC_NONE},
  [TOKEN_AND] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE] = {read_literal,     NULL,   PREC_NONE},
  //[TOKEN_FOR]				= {NULL,     NULL,   PREC_NONE},
  [TOKEN_METHOD] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF] = {NULL,     NULL,   PREC_NONE},
  //[TOKEN_NIL]				= {NULL,     NULL,   PREC_NONE},
  [TOKEN_OR] = {NULL,     NULL,   PREC_NONE},
  //[TOKEN_PRINT]			= {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BASE] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE] = {read_literal,     NULL,   PREC_NONE},
  //[TOKEN_VAR]				= {NULL,     NULL,   PREC_NONE},
  //[TOKEN_WHILE]			= {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF] = {NULL,     NULL,   PREC_NONE},
};

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
		parser.current = scan_next_token();
		if (parser.current.type != TOKEN_ERROR) break;

		errorAtCurrent(parser.current.start);
	}
}

static bool match(TokenType type)
{
	if (!check_curr_type(type)) return false;
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

void emit_byte(KCompiler* compiler, Byte1 byte)
{
	bytec_write(compiler->bytec, byte, compiler->parser.previous.line);
}

void emit_2bytes(KCompiler* compiler, Byte2 bytes)
{
	emit_byte(compiler, bytes >> 8);
	emit_byte(compiler, bytes);
}

void emit_4bytes(KCompiler* compiler, Byte4 bytes)
{
	emit_byte(compiler, bytes >> 24);
	emit_byte(compiler, bytes >> 16);
	emit_byte(compiler, bytes >> 8);
	emit_byte(compiler, bytes);
}

void emitReturn(KCompiler* compiler)
{
	emit_byte(compiler, OP_RETURN);
}

void kcom_end(KCompiler* compiler)
{
	emitReturn(compiler);

#ifdef DEBUG_PRINT_CODE
	if (!compiler->parser.hadError)
	{
		debug_disassemble_bytec(compiler->bytec, "Compiled Bytecode");
	}
#endif
}

static uint8_t make_const(Byte1* byte, TYPE_ID id)
{
	int constant = ByteCode_AddConstant(currentChunk(), id, byte);
	if (constant > UINT8_MAX)
	{
		errorAtPrevious("Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}

static uint8_t make_constp(Byte1* byte, TYPE_ID id)
{
	int constant = ByteCode_AddConstantPointer(currentChunk(), id, byte);
	if (constant > UINT8_MAX)
	{
		errorAtPrevious("Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}

static void emitConstant(Byte1* bytes, TYPE_ID id)
{
	emit3Bytes(OP_CONSTANT, id, make_const(bytes, id));
}

static void emitConstantPointer(Byte1* bytes, TYPE_ID id)
{
	emit3Bytes(OP_CONSTANT, id, make_constp(bytes, id));
}

static void printStatement()
{
	read_expression();
	consume(TOKEN_SEMICOLON, "Expect ';' after value.");
	emit_byte(OP_PRINT);
}

static void expressionStatement()
{
	read_expression();
	consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
	emit_byte(OP_POP);
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
	return make_constp(string, type);
}

static void defineVariable(TYPE_ID type, uint8_t global)
{
	emit3Bytes(OP_DEFINE_GLOBAL, global, type);
}

static uint8_t parseVariable(TYPE_ID type, const char* errorMessage)
{
	consume(TOKEN_IDENTIFIER, errorMessage);
	return identifierConstant(type, &parser.previous);
}

static void varDeclaration(TYPE_ID type)
{
	uint8_t global = parseVariable(type, "Expect variable name.");

	if (match(TOKEN_EQUALS))
	{
		read_expression();
	}

	consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

	defineVariable(type, global);
}

//static void namedVariable(Token name) 
//{
//	uint8_t arg = identifierConstant(&name);
//	emitBytes(OP_GET_GLOBAL, arg);
//}

void read_literal(KCompiler* compiler)
{
	switch (compiler->parser.previous.type)
	{
	case TOKEN_TRUE: emit_byte(OP_TRUE); break;
	case TOKEN_FALSE: emit_byte(OP_FALSE); break;
	}
}

void read_string(KCompiler* compiler)
{
	StringPointer* s = CopyString(compiler->parser.previous.start + 1, compiler->parser.previous.length - 2);
	emitConstantPointer(s, TYPEID_STRING);
}

void read_number(KCompiler* compiler)
{
	double value = strtod(compiler->parser.previous.start, NULL);
	emitConstant(&value, TYPEID_DEC);
}

//void variable() 
//{
//	namedVariable(parser.previous);
//}

void read_unary(KCompiler* compiler)
{
	TokenType operatorType = compiler->parser.previous.type;

	// Compile the operand.
	parsePrecedence(PREC_UNARY);

	// Emit the operator instruction.
	switch (operatorType)
	{
	case TOKEN_MINUS: emit_byte(OP_NEGATE); break;
	case TOKEN_NOT: emit_byte(OP_NOT); break;
	default: return;
	}
}

void read_binary(KCompiler* compiler)
{
	TokenType operatorType = compiler->parser.previous.type;
	ParseRule* rule = getRule(operatorType);
	parsePrecedence((Precedence)(rule->precedence + 1));

	switch (operatorType)
	{
	case TOKEN_PLUS:				emit_byte(OP_ADD); break;
	case TOKEN_MINUS:				emit_byte(OP_SUBTRACT); break;
	case TOKEN_STAR:				emit_byte(OP_MULT); break;
	case TOKEN_SLASH:				emit_byte(OP_DIVIDE); break;
	case TOKEN_EQUALS:				emit_byte(OP_EQUALS); break;

		//case TOKEN_IS_NOT:				emit2Bytes(OP_EQUAL, OP_NOT); break;
		//case TOKEN_IS:					emitByte(OP_IS); break;	To define equality on types
	case TOKEN_GREATER:				emit_byte(OP_GREAT); break;
	case TOKEN_GREATER_OR_EQUALS:	emit_2bytes(OP_LESS, OP_NOT); break;
	case TOKEN_LESS:				emit_byte(OP_LESS); break;
	case TOKEN_LESS_OR_EQUALS:		emit_2bytes(OP_GREAT, OP_NOT); break;
	default: return;
	}
}

void read_expression(KCompiler* compiler)
{
	parsePrecedence(PREC_ASSIGNMENT);
}

void read_statement(KCompiler* compiler)
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

void read_declaration(KCompiler* compiler)
{
	if (match(TOKEN_IDENTIFIER))
	{
		TYPE_ID type = TypeTable_GetTypeId(compiler->parser.previous.start, compiler->parser.previous.length);
		if (type != TYPEID_VOID)
			varDeclaration(type);
		else
			errorAtCurrent("%s is not defined.", TypeTable_GetTypeInfo(type)->name);
	}
	else
	{
		read_statement(compiler);
	}

	if (compiler->parser.isPanic) synchronize();
}

void read_grouping(KCompiler* compiler)
{
	read_expression(compiler);
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

bool kcom_compile(const char* source, ByteCode* chunk)
{
	KCompiler compiler;
	compiler.chunk = chunk;

	Scanner scanner;
	scan_init(&scanner, source);

	compiler.parser.hadError = false;
	compiler.parser.isPanic = false;

	advanceToken();

	while (!match(TOKEN_EOF))
	{
		read_declaration(&compiler);
	}

	consume(TOKEN_EOF, "Expect end of expression.");
	kcom_end();
	return !compiler.parser.hadError;
}
