#include "kparser.h"
#include <stdio.h>

void error_at_tkn(Parser* parser, Token* token, const char* message)
{
	if (parser->isPanic) return;

	parser->isPanic = true;
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
	parser->hadError = true;
}

void error_prev(Parser* parser, const char* message)
{
	error_at_tkn(parser, &parser->previous, message);
}

void error_curr(Parser* parser, const char* message)
{
	error_at_tkn(parser, &parser->current, message);
}

void parser_init(Parser* parser, const char* source)
{
	parser->isPanic = false;
	parser->hadError = false;

	scan_init(&parser->scanner, source);
}

bool parser_check_curr_type(Parser* parser, TokenType type)
{
	return parser->current.type == type;
}

void parser_adv_tkn(Parser* parser)
{
	parser->previous = parser->current;

	while (true)
	{
		parser->current = scan_next_token(&parser->scanner);
		if (parser->current.type == TOKEN_ERROR) error_curr(parser, parser->current.start);
		else break;
	}
}

void parser_sync(Parser* parser)
{
	parser->isPanic = false;

	while (parser->current.type != TOKEN_EOF)
	{
		if (parser->previous.type == TOKEN_SEMICOLON) return;
		switch (parser->current.type)
		{
		case TOKEN_CLASS:
		case TOKEN_IF:
		case TOKEN_PRINT:
		case TOKEN_RETURN:
			return;

		default:
			; // Do nothing.
		}

		parser_adv_tkn(parser);
	}
}

void parser_precedence(Parser* parser, ParseRule* parseRules, Precedence precedence)
{
	parser_adv_tkn(parser);
	ParseFn prefixRule = get_rule(parseRules, parser->previous.type)->prefix;
	if (prefixRule == NULL)
	{
		error_prev(parser, "Expect expression.");
		return;
	}

	prefixRule(parser);

	while (precedence <= get_rule(parseRules, parser->current.type)->precedence)
	{
		parser_adv_tkn(parser);
		ParseFn infixRule = get_rule(parseRules, parser->previous.type)->infix;
		infixRule(parser);
	}
}

void parser_advmatch_or_error(Parser* parser, TokenType type, const char* message)
{
	if (parser->current.type == type)
	{
		parser_adv_tkn(parser);
		return;
	}

	error_curr(parser, message);
}

bool parser_advmatch(Parser* parser, TokenType type)
{
	if (!parser_check_curr_type(parser, type)) return false;
	parser_adv_tkn(parser);
	return true;
}
