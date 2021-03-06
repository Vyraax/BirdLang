#include "pch.h"
#include "Token.h"

std::vector<std::string> Token::keywords = {
	"var",
	"and",
	"or",
	"not",
	"if",
	"then",
	"else",
	"elif",
	"for",
	"while",
	"to",
	"step",
	"function",
	"import",
	"break",
	"return",
	"continue",
	"delete"
};

Token::Token(
	const Token::Type& type,
	const std::variant<double, int, char, std::string>& value,
	std::shared_ptr<Cursor> start,
	std::shared_ptr<Cursor> end
) :
    type(type), 
    value(value),
    start(start),
    end(end)
{
    if (start != nullptr) {
        this->start = start;
        this->end = start;
        this->end->advance();
    }

	if (end != nullptr) {
		this->end = end;
	}
}
