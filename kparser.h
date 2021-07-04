#ifndef __KPARSER_HEADER__
#define __KPARSER_HEADER__

#include "common.h"
#include "kscan.h"

//Helps the compiler to interpret tokens into op_codes
typedef struct Parser {
	Scanner scanner;
	Token current;
	Token previous;
	bool hadError;
	bool isPanic;
} Parser;

void parser_init(Parser* parser, const char* source);
bool parser_check_curr_type(Parser* parser, TokenType type);
void parser_adv_tkn(Parser* parser);
void parser_sync(Parser* parser);
void parser_advmatch_or_error(Parser* parser, TokenType type, const char* message);
bool parser_advmatch(Parser* parser, TokenType type);
void parser_error_prev(Parser* parser, const char* message);
void parser_error_curr(Parser* parser, const char* message);

#endif