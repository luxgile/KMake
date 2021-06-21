#pragma once

typedef enum {
	// Single-character tokens.
	TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
	TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
	TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
	TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

	//Logicals
	TOKEN_AND, TOKEN_EQUALS, TOKEN_GREATER, TOKEN_LESS, TOKEN_GREATER_OR_EQUALS, TOKEN_LESS_OR_EQUALS,
	TOKEN_OR, TOKEN_NOT, TOKEN_NOT_EQUALS,

	// Literals.
	TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

	// Keywords.
	TOKEN_CLASS, TOKEN_METHOD, TOKEN_ELSE, TOKEN_FALSE,
	TOKEN_IF, TOKEN_RETURN, TOKEN_BASE, TOKEN_THIS,
	TOKEN_TRUE,

	TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct
{
	TokenType type;
	const char* start;
	int length;
	int line;
} Token;

typedef struct
{
	const char* start;
	const char* current;
	int line;
} Scanner;


Scanner scanner;

void initScanner(const char* source);
Token scanToken();