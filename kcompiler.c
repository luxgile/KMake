#include "kcompiler.h"
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "kscan.h"
#include "kdebug.h"
#include "bytecode.h"
#include "commonTypes.h"
#include "vm.h"
#include "kparser.h"

#define GET_TYPE_SIZE(compiler, type) (typetbl_get_info(&compiler->bytec->typeTable, type)->size)

void read_grouping(KCompiler* compiler);
void read_unary(KCompiler* compiler);
void read_binary(KCompiler* compiler);
void read_expression(KCompiler* compiler);
void read_statement(KCompiler* compiler);
void read_declaration(KCompiler* compiler);
void read_number(KCompiler* compiler);
void read_literal(KCompiler* compiler);
//void read_string(KCompiler* compiler);

//Move to different file
ParseRule g_parseRules[] = {
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
  //[TOKEN_STRING] = {read_string,     NULL,   PREC_NONE},
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

ParseRule* get_rule(ParseRule* parseRules, TokenType type) {
	return &parseRules[type];
}

inline void emit_byte(KCompiler* compiler, Byte1 byte) {
	bytec_write(compiler->bytec, byte, compiler->parser.previous.line);
}

void emit_2bytes(KCompiler* compiler, Byte2 bytes) {
	emit_byte(compiler, bytes >> 8);
	emit_byte(compiler, (Byte1)bytes);
}

void emit_4bytes(KCompiler* compiler, Byte4 bytes) {
	emit_byte(compiler, bytes >> 24);
	emit_byte(compiler, bytes >> 16);
	emit_byte(compiler, bytes >> 8);
	emit_byte(compiler, bytes);
}

void emitReturn(KCompiler* compiler) {
	emit_byte(compiler, OP_RETURN);
}

void kcom_end(KCompiler* compiler) {
	emitReturn(compiler);

	#ifdef DEBUG_PRINT_CODE
	if (!compiler->parser.hadError) {
		debug_disassemble_bytec(compiler->bytec, "Compiled Bytecode");
	}
	#endif
}

uint8_t make_const(KCompiler* compiler, Byte1* byte, TYPE_ID id) {
	int constant = bytec_add_c(compiler->bytec, byte, id, GET_TYPE_SIZE(compiler, id));
	if (constant > UINT8_MAX) {
		parser_error_prev(&compiler->parser, "Too many constants in one chunk.");
		return 0;
	}

	return (uint8_t)constant;
}

//uint8_t make_constp(KCompiler* compiler, Byte1* byte, TYPE_ID id) {
//	int constant = bytec_add_cp(compiler->bytec, id, byte);
//	if (constant > UINT8_MAX) {
//		error_prev(&compiler->parser, "Too many constants in one chunk.");
//		return 0;
//	}
//
//	return (uint8_t)constant;
//}

void emit_const(KCompiler* compiler, Byte1* bytes, TYPE_ID id) {
	emit_byte(compiler, OP_CONSTANT);
	emit_byte(compiler, id);
	emit_byte(compiler, make_const(compiler, bytes, id));
}

//void emit_constp(KCompiler* compiler, Byte1* bytes, TYPE_ID id) {
//	emit_byte(compiler, OP_CONSTANT);
//	emit_byte(compiler, id);
//	emit_byte(compiler, make_constp(compiler, bytes, id));
//}

void statement_print(KCompiler* compiler) {
	read_expression(compiler);
	parser_advmatch_or_error(&compiler->parser, TOKEN_SEMICOLON, "Expect ';' after value.");
	emit_byte(compiler, OP_PRINT);
}

void statement_exprs(KCompiler* compiler) {
	read_expression(compiler);
	parser_advmatch_or_error(&compiler->parser, TOKEN_SEMICOLON, "Expect ';' after expression.");
	emit_byte(compiler, OP_POP);
}

static void __make_precedence(KCompiler* compiler, ParseRule* parseRules, Precedence precedence) {
	parser_adv_tkn(&compiler->parser);
	ParseFn prefixRule = get_rule(parseRules, compiler->parser.previous.type)->prefix;
	if (prefixRule == NULL) {
		parser_error_prev(&compiler->parser, "Expect expression.");
		return;
	}

	prefixRule(compiler);

	while (precedence <= get_rule(parseRules, compiler->parser.current.type)->precedence) {
		parser_adv_tkn(&compiler->parser);
		ParseFn infixRule = get_rule(parseRules, compiler->parser.previous.type)->infix;
		infixRule(compiler);
	}
}

//uint8_t make_identifier(KCompiler* compiler, TYPE_ID type, Token* name) {
//	StringPointer* string = CopyString(name->start, name->length);
//	return make_constp(compiler, string, type);
//}

//void var_define(TYPE_ID type, uint8_t global) {
//	emit3Bytes(OP_DEFINE_GLOBAL, global, type);
//}

//uint8_t var_parse(KCompiler* compiler, TYPE_ID type, const char* errorMessage) {
//	parser_advmatch_or_error(&compiler->parser, TOKEN_IDENTIFIER, errorMessage);
//	return make_identifier(compiler, type, &compiler->parser.previous);
//}
//
//void var_declaration(KCompiler* compiler, TYPE_ID type) {
//	uint8_t global = var_parse(compiler, type, "Expect variable name.");
//
//	if (parser_advmatch(&compiler->parser, TOKEN_EQUALS)) {
//		read_expression(compiler);
//	}
//
//	parser_advmatch_or_error(&compiler->parser, TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
//
//	var_define(type, global);
//}

void read_literal(KCompiler* compiler) {
	switch (compiler->parser.previous.type) {
	case TOKEN_TRUE: emit_byte(compiler, OP_TRUE); break;
	case TOKEN_FALSE: emit_byte(compiler, OP_FALSE); break;
	default: break;
	}
}

//void read_string(KCompiler* compiler) {
//	StringPointer* s = CopyString(compiler->parser.previous.start + 1, compiler->parser.previous.length - 2);
//	emit_constp(compiler, s, TYPEID_STRING);
//}

void read_number(KCompiler* compiler) {
	double value = strtod(compiler->parser.previous.start, NULL);
	emit_const(compiler, (Byte1*)&value, TYPEID_DEC);
}

void read_unary(KCompiler* compiler) {
	TokenType operatorType = compiler->parser.previous.type;

	// Compile the operand.
	__make_precedence(compiler, g_parseRules, PREC_UNARY);

	// Emit the operator instruction.
	switch (operatorType) {
	case TOKEN_MINUS: emit_byte(compiler, OP_NEGATE); break;
	case TOKEN_NOT: emit_byte(compiler, OP_NOT); break;
	default: return;
	}
}

void read_binary(KCompiler* compiler) {
	TokenType operatorType = compiler->parser.previous.type;
	ParseRule* rule = get_rule(g_parseRules, operatorType);
	__make_precedence(compiler, g_parseRules, (Precedence)(rule->precedence + 1));

	switch (operatorType) {
	case TOKEN_PLUS:				emit_byte(compiler, OP_ADD); break;
	case TOKEN_MINUS:				emit_byte(compiler, OP_SUBTRACT); break;
	case TOKEN_STAR:				emit_byte(compiler, OP_MULT); break;
	case TOKEN_SLASH:				emit_byte(compiler, OP_DIVIDE); break;
	case TOKEN_EQUALS:				emit_byte(compiler, OP_EQUALS); break;

		/*case TOKEN_IS_NOT:				emit2Bytes(OP_EQUAL, OP_NOT); break;
		case TOKEN_IS:					emitByte(OP_IS); break;	To define equality on types*/
	case TOKEN_GREATER:				emit_byte(compiler, OP_GREAT); break;
	case TOKEN_GREATER_OR_EQUALS:	emit_byte(compiler, OP_LESS); emit_byte(compiler, OP_NOT); break;
	case TOKEN_LESS:				emit_byte(compiler, OP_LESS); break;
	case TOKEN_LESS_OR_EQUALS:		emit_byte(compiler, OP_GREAT); emit_byte(compiler, OP_NOT); break;
	default: return;
	}
}

void read_expression(KCompiler* compiler) {
	__make_precedence(compiler, g_parseRules, PREC_ASSIGNMENT);
}

void read_statement(KCompiler* compiler) {
	if (parser_advmatch(&compiler->parser, TOKEN_PRINT)) {
		statement_print(compiler);
	}
	else {
		statement_exprs(compiler);
	}
}

void read_declaration(KCompiler* compiler) {
	/*if (parser_advmatch(&compiler->parser, TOKEN_IDENTIFIER)) {
		TYPE_ID type = typetbl_get_id(&compiler->bytec->typeTable, compiler->parser.previous.start, compiler->parser.previous.length);
		if (type != TYPEID_VOID)
			var_declaration(compiler, type);asdasdasdasd
		else
			parser_error_curr("%s is not defined.", typetbl_get_info(&compiler->bytec->typeTable, type)->name);
	}
	else {
		read_statement(compiler);
	}*/
	read_statement(compiler);

	if (compiler->parser.isPanic) parser_sync(&compiler->parser);
}

void read_grouping(KCompiler* compiler) {
	read_expression(compiler);
	parser_advmatch_or_error(&compiler->parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

bool kcom_compile(const char* source, ByteCode* chunk) {
	KCompiler compiler;
	compiler.bytec = chunk;
	parser_init(&compiler.parser, source);

	compiler.parser.hadError = false;
	compiler.parser.isPanic = false;

	parser_adv_tkn(&compiler.parser);

	while (!parser_advmatch(&compiler.parser, TOKEN_EOF)) {
		read_declaration(&compiler);
	}

	parser_advmatch_or_error(&compiler.parser, TOKEN_EOF, "Expect end of expression.");
	kcom_end(&compiler);
	return !compiler.parser.hadError;
}