#include "kscan.h"
#include <stdio.h>
#include <string.h>
#include "common.h"

static bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}

static bool is_letter(char c)
{
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

Token make_token_from_curr(Scanner* scanner, TokenType type)
{
	Token token;
	token.type = type;
	token.start = scanner->start;
	token.length = (int)(scanner->current - scanner->start);
	token.line = scanner->line;
	return token;
}

Token make_errortok_from_curr(Scanner* scanner, const char* msg)
{
	Token token;
	token.type = TOKEN_ERROR;
	token.start = msg;
	token.length = (int)strlen(msg);
	token.line = scanner->line;
	return token;
}

char adv_char(Scanner* scanner)
{
	scanner->current++;
	return scanner->current[-1];
}

bool is_at_end(Scanner* scanner)
{
	return *scanner->current == '\0';
}


//bool match(Scanner* scanner, char expected)
//{
//	if (is_at_end(scanner)) return false;
//
//	if (*scanner->current != expected) return false;
//
//	scanner->current++;
//	return true;
//}

char peek_curr_char(Scanner* scanner)
{
	return *scanner->current;
}

char peek_next_char(Scanner* scanner)
{
	if (is_at_end(scanner))
	{
		return '\0';
	}

	return scanner->current[1];
}

void skip_empty(Scanner* scanner)
{
	while (true)
	{
		char c = peek_curr_char(scanner);
		switch (c)
		{
		case ' ':
		case '\r':
		case '\t':
			adv_char(scanner);
			break;

		case '\n':
		{
			scanner->line++;
			adv_char(scanner);
			break;
		}

		case '/':
		{
			// A comment goes until the end of the line.
			if (peek_next_char(scanner) == '/') while (peek_curr_char(scanner) != '\n' && !is_at_end(scanner)) adv_char(scanner);
			else return;
			break;
		}

		default:
			return;
		}
	}
}

Token tkn_make_string(Scanner* scanner)
{
	while (peek_curr_char(scanner) != '"' && !is_at_end(scanner))
	{
		if (peek_curr_char(scanner) == '\n') scanner->line++;
		adv_char(scanner);
	}

	if (is_at_end(scanner)) return make_errortok_from_curr(scanner, "Unterminated string.");

	// The closing quote.
	adv_char(scanner);
	return make_token_from_curr(scanner, TOKEN_STRING);
}

Token tkn_make_number(Scanner* scanner)
{
	while (is_digit(peek_curr_char(scanner))) adv_char(scanner);

	// Look for a fractional part.
	if (peek_curr_char(scanner) == '.' && is_digit(peek_next_char(scanner)))
	{
		// Consume the ".".
		adv_char(scanner);

		while (is_digit(peek_curr_char(scanner))) adv_char(scanner);
	}

	return make_token_from_curr(scanner, TOKEN_NUMBER);
}

static TokenType check_keyword(Scanner* scanner, int start, int length, const char* rest, TokenType type)
{
	if (scanner->current - scanner->start == start + length &&
		memcmp(scanner->start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_IDENTIFIER;
}

//TODO: Change this big chunk of switches to a trie
static TokenType get_identifier_type(Scanner* scanner)
{
	switch (scanner->start[0])
	{
	case 'a': return check_keyword(scanner, 1, 2, "nd", TOKEN_AND);
	case 'c': return check_keyword(scanner, 1, 4, "lass", TOKEN_CLASS);
	case 'e':
	{
		if (scanner->current - scanner->start > 1)
		{
			switch (scanner->start[1])
			{
			case 'l': return check_keyword(scanner, 2, 2, "se", TOKEN_ELSE);
			case 'q': return check_keyword(scanner, 2, 4, "uals", TOKEN_EQUALS);
			}
		}
	}
	case 'i': return check_keyword(scanner, 1, 1, "f", TOKEN_IF);
	case 'p': return check_keyword(scanner, 1, 4, "rint", TOKEN_PRINT);
	case 'o': return check_keyword(scanner, 1, 1, "r", TOKEN_OR);
	case 'r': return check_keyword(scanner, 1, 2, "et", TOKEN_RETURN);
	case 'b': return check_keyword(scanner, 1, 3, "ase", TOKEN_BASE);
	case 'f': return check_keyword(scanner, 1, 4, "alse", TOKEN_FALSE);
	case 'm': return check_keyword(scanner, 1, 5, "ethod", TOKEN_METHOD);
	case 'g': return check_keyword(scanner, 1, 4, "reat", TOKEN_GREATER);
	case 'l': return check_keyword(scanner, 1, 3, "ess", TOKEN_LESS);
	case 't':
		if (scanner->current - scanner->start > 1)
		{
			switch (scanner->start[1])
			{
			case 'h': return check_keyword(scanner, 2, 2, "is", TOKEN_THIS);
			case 'r': return check_keyword(scanner, 2, 2, "ue", TOKEN_TRUE);
			}
		}
		break;

	case 'n':
		if (scanner->current - scanner->start > 1)
		{
			switch (scanner->start[1])
			{
			case 'o': return check_keyword(scanner, 2, 1, "t", TOKEN_NOT);
			}
		}
	}

	return TOKEN_IDENTIFIER;
}

static Token tkn_make_identifier(Scanner* scanner)
{
	while (is_letter(peek_curr_char(scanner)) || is_digit(peek_curr_char(scanner))) adv_char(scanner);

	Token token = make_token_from_curr(get_identifier_type(scanner));
	/*switch (token.type)
	{
	case TOKEN_IS:
	}*/
	return token;
}

void scan_init(Scanner* scanner, const char* source)
{
	scanner->start = source;
	scanner->current = source;
	scanner->line = 1;
}

Token scan_next_token(Scanner* scanner)
{
	skip_empty(scanner);

	scanner->start = scanner->current;

	if (is_at_end(scanner)) return make_token_from_curr(scanner, TOKEN_EOF);

	char c = adv_char(scanner);

	if (is_digit(c)) return tkn_make_number(scanner);
	if (is_letter(c)) return tkn_make_identifier(scanner);

	switch (c)
	{
	case '(': return make_token_from_curr(scanner, TOKEN_LEFT_PAREN);
	case ')': return make_token_from_curr(scanner, TOKEN_RIGHT_PAREN);
	case '{': return make_token_from_curr(scanner, TOKEN_LEFT_BRACE);
	case '}': return make_token_from_curr(scanner, TOKEN_RIGHT_BRACE);
	case ';': return make_token_from_curr(scanner, TOKEN_SEMICOLON);
	case ',': return make_token_from_curr(scanner, TOKEN_COMMA);
	case '.': return make_token_from_curr(scanner, TOKEN_DOT);
	case '+': return make_token_from_curr(scanner, TOKEN_PLUS);
	case '-': return make_token_from_curr(scanner, TOKEN_MINUS);
	case '*': return make_token_from_curr(scanner, TOKEN_STAR);
	case '/': return make_token_from_curr(scanner, TOKEN_SLASH);
	case '"': return tkn_make_string(scanner);
	}

	return make_errortok_from_curr("Unexpected character.");
}
