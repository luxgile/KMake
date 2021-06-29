#include "kparser.h"

bool check_curr_type(Parser* parser, TokenType type)
{
	return parser->current.type == type;
}
