#include "kscan.h"
#include <stdio.h>
#include <string.h>
#include "common.h"

static bool isAtEnd()
{
	return *scanner.current == '\0';
}

static bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}

static bool isLetter(char c)
{
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

static Token makeToken(TokenType type)
{
	Token token;
	token.type = type;
	token.start = scanner.start;
	token.length = (int)(scanner.current - scanner.start);
	token.line = scanner.line;
	return token;
}

static Token errorToken(const char* msg)
{
	Token token;
	token.type = TOKEN_ERROR;
	token.start = msg;
	token.length = (int)strlen(msg);
	token.line = scanner.line;
	return token;
}

static char charAdvance()
{
	scanner.current++;
	return scanner.current[-1];
}

static bool match(char expected)
{
	if (isAtEnd()) return false;
	if (*scanner.current != expected) return false;
	scanner.current++;
	return true;
}

static char peek()
{
	return *scanner.current;
}

static char peekNext()
{
	if (isAtEnd())
	{
		return '\0';
	}

	return scanner.current[1];
}

static void skipWhitespace()
{
	while (true)
	{
		char c = peek();
		switch (c)
		{
		case ' ':
		case '\r':
		case '\t':
			charAdvance();
			break;

		case '\n':
		{
			scanner.line++;
			charAdvance();
			break;
		}

		case '/':
		{
			if (peekNext() == '/')
			{
				// A comment goes until the end of the line.
				while (peek() != '\n' && !isAtEnd())
				{
					charAdvance();
				}
			}
			else
			{
				return;
			}
			break;
		}

		default:
			return;
		}
	}
}

static Token makeString()
{
	while (peek() != '"' && !isAtEnd())
	{
		if (peek() == '\n') scanner.line++;
		charAdvance();
	}

	if (isAtEnd())
	{
		return errorToken("Unterminated string.");
	}

	// The closing quote.
	scanner.current++;
	return makeToken(TOKEN_STRING);
}

static Token makeNumber()
{
	while (isDigit(peek())) charAdvance();

	// Look for a fractional part.
	if (peek() == '.' && isDigit(peekNext()))
	{
		// Consume the ".".
		charAdvance();

		while (isDigit(peek())) charAdvance();
	}

	return makeToken(TOKEN_NUMBER);
}

static TokenType checkKeyword(int start, int length, const char* rest, TokenType type)
{
	if (scanner.current - scanner.start == start + length &&
		memcmp(scanner.start + start, rest, length) == 0) {
		return type;
	}

	return TOKEN_IDENTIFIER;
}

static TokenType identifierType()
{
	switch (scanner.start[0])
	{
	case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
	case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
	case 'e':
	{
		if (scanner.current - scanner.start > 1)
		{
			switch (scanner.start[1])
			{
			case 'l': return checkKeyword(2, 2, "se", TOKEN_ELSE);
			case 'q': return checkKeyword(2, 4, "uals", TOKEN_EQUALS);
			}
		}
	}
	case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
	case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
	case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
	case 'r': return checkKeyword(1, 2, "et", TOKEN_RETURN);
	case 'b': return checkKeyword(1, 3, "ase", TOKEN_BASE);
	case 'f': return checkKeyword(1, 4, "alse", TOKEN_FALSE);
	case 'm': return checkKeyword(1, 5, "ethod", TOKEN_METHOD);
	case 'g': return checkKeyword(1, 4, "reat", TOKEN_GREATER);
	case 'l': return checkKeyword(1, 3, "ess", TOKEN_LESS);
	case 't':
		if (scanner.current - scanner.start > 1)
		{
			switch (scanner.start[1])
			{
			case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
			case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
			}
		}
		break;

	case 'n':
		if (scanner.current - scanner.start > 1)
		{
			switch (scanner.start[1])
			{
			case 'o': return checkKeyword(2, 1, "t", TOKEN_NOT);
			}
		}
	}

	return TOKEN_IDENTIFIER;
}

static Token makeIdentifier()
{
	while (isLetter(peek()) || isDigit(peek())) charAdvance();

	Token token = makeToken(identifierType());
	/*switch (token.type)
	{
	case TOKEN_IS:
	}*/
	return token;
}

void initScanner(const char* source)
{
	scanner.start = source;
	scanner.current = source;
	scanner.line = 1;
}


Token scanToken()
{
	skipWhitespace();

	scanner.start = scanner.current;

	if (isAtEnd()) return makeToken(TOKEN_EOF);

	char c = charAdvance();

	if (isDigit(c)) return makeNumber();
	if (isLetter(c)) return makeIdentifier();

	switch (c)
	{
	case '(': return makeToken(TOKEN_LEFT_PAREN);
	case ')': return makeToken(TOKEN_RIGHT_PAREN);
	case '{': return makeToken(TOKEN_LEFT_BRACE);
	case '}': return makeToken(TOKEN_RIGHT_BRACE);
	case ';': return makeToken(TOKEN_SEMICOLON);
	case ',': return makeToken(TOKEN_COMMA);
	case '.': return makeToken(TOKEN_DOT);
	case '+': return makeToken(TOKEN_PLUS);
	case '-': return makeToken(TOKEN_MINUS);
	case '*': return makeToken(TOKEN_STAR);
	case '/': return makeToken(TOKEN_SLASH);
	case '"': return makeString();
	}

	return errorToken("Unexpected character.");
}
