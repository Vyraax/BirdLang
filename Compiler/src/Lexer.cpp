#include "pch.h"
#include <sstream>
#include "Lexer.h"
#include "Profiler.h"
#include "ConsoleTable.h"
#include "Utils.h"

using ConsoleTable = samilton::ConsoleTable;

const std::string digits = "0123456789";
const std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string letters_digits = letters + digits;

Lexer::Lexer(const std::string& filename, bool debug) : 
	filename(filename),
	tokens({}),
	current_char('\0'),
	debug(false),
	lexing_time(0.0)
{
	cursor = std::make_shared<Cursor>(-1, 0, -1, filename, input);
}

void Lexer::advance()
{
	cursor->advance(current_char);

	current_char = cursor->index < input.size()
		? input.at(cursor->index)
		: '\0';
}

void Lexer::create_token(const Token::Type& type, char value)
{
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
	advance();
	tokens.push_back(new Token(type, value, start, cursor));
}

std::vector<Token*> Lexer::index_tokens(const std::string& str)
{
	Profiler profiler;
	profiler.start = clock();
	this->input = str;
	tokens.clear();
	cursor.reset(new Cursor(-1, 0, -1, filename, input));
	advance();

	while (current_char != '\0') {
		if (current_char == ' ' || current_char == '\t') {
			advance();
		}
		else if (digits.find(current_char) != std::string::npos) {
			tokens.push_back(create_numeric_token());
		}
		else if (letters.find(current_char) != std::string::npos) {
			tokens.push_back(create_identifier());
		}
		else if (current_char == '"') {
			tokens.push_back(create_string());
		}
		else if (current_char == '+') {
			create_token(Token::Type::PLUS, current_char);
		}
		else if (current_char == '-') {
			tokens.push_back(create_minus_arrow_operator());
		}
		else if (current_char == '*') {
			create_token(Token::Type::MUL, current_char);
		}
		else if (current_char == '%') {
			create_token(Token::Type::MOD, current_char);
		}
		else if (current_char == '/') {
			create_token(Token::Type::DIV, current_char);
		}
		else if (current_char == '^') {
			create_token(Token::Type::POW, current_char);
		}
		else if (current_char == '(') {
			create_token(Token::Type::LPAREN, current_char);
		}
		else if (current_char == ')') {
			create_token(Token::Type::RPAREN, current_char);
		}
		else if (current_char == '[') {
			create_token(Token::Type::LSBRACKET, current_char);
		}
		else if (current_char == ']') {
			create_token(Token::Type::RSBRACKET, current_char);
		}
		else if (current_char == '{') {
			create_token(Token::Type::LCBRACKET, current_char);
		}
		else if (current_char == '}') {
			create_token(Token::Type::RCBRACKET, current_char);
		}
		else if (current_char == ':') {
			create_token(Token::Type::COLON, current_char);
		}
		else if (current_char == ';') {
			create_token(Token::Type::SEMI_COLON, current_char);
		}
		else if (current_char == '!') {
			Token* token = create_not_equals_operator();

			if (token == nullptr) {
				std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
				advance();
				
				ExpectedCharacterError* error = new ExpectedCharacterError(
					start, 
					cursor, 
					"'=' (after '!')"
				);
				
				std::cout << error << "\n";

				return std::vector<Token*>();
			}

			tokens.push_back(token);
		}
		else if (current_char == '=') {
			tokens.push_back(create_equals_operator());
		}
		else if (current_char == '<') {
			tokens.push_back(create_less_operator());
		}
		else if (current_char == '>') {
			tokens.push_back(create_greater_operator());
		}
		else if (current_char == ',') {
			create_token(Token::Type::COMMA, current_char);
		}
		else if (current_char == '.') {
			create_token(Token::Type::DOT, current_char);
		}
		else {
			std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
			char c = current_char;
			advance();

			IllegarCharError* error = new IllegarCharError(
				start, 
				cursor,
				std::string(1, c)
			);
			
			std::cout << error << "\n";

			return std::vector<Token*>();
		}
	}

	cursor->column = (int)tokens.size();
	tokens.push_back(new Token(Token::Type::EOL, char(0x04)));
	profiler.end = clock();
	lexing_time = profiler.getReport();

	if (debug) {
		ConsoleTable table(1, 2);
		ConsoleTable::TableChars chars;

		chars.topLeft = '+';
		chars.topRight = '+';
		chars.downLeft = '+';
		chars.downRight = '+';
		chars.topDownSimple = '-';
		chars.leftRightSimple = '|';
		chars.leftSeparation = '+';
		chars.rightSeparation = '+';
		chars.centreSeparation = '+';
		chars.topSeparation = '+';
		chars.downSeparation = '+';

		table.setTableChars(chars);

		table[0][0] = "Identifier";
		table[0][1] = "Character";
		table[0][2] = "Line";
		table[0][3] = "Column";

		for (unsigned int i = 0; i < tokens.size(); i++) {
			auto token = tokens.at(i);
			std::stringstream str;

			switch (token->value.index()) {
			case 0:
				try { str << std::get<double>(token->value); }
				catch (const std::bad_variant_access&) {}
				break;
			case 1:
				try { str << std::get<int>(token->value); }
				catch (const std::bad_variant_access&) {}
				break;
			case 2:
				try { str << std::get<char>(token->value); }
				catch (const std::bad_variant_access&) {}
				break;
			case 3:
				try { str << std::get<std::string>(token->value); }
				catch (const std::bad_variant_access&) {}
				break;
			}
			
			table[i + 1][0] = Token::toString(token->type);
			table[i + 1][1] = str.str();

			size_t line = 0;
			size_t column = tokens.size();

			if (token->start != nullptr) {
				line = token->start->line;
				column = token->start->column;
			}

			table[i + 1][2] = std::to_string(line);
			table[i + 1][3] = std::to_string(column);
		}

		Utils::title("TOKENS", 32, false);
		std::cout << table << "\n";
	}

	return tokens;
}

Token* Lexer::create_numeric_token()
{
	std::string str;
	unsigned int dots = 0;
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
	auto numbers = digits + ".";

	auto hasDot = [=]() { 
		return numbers.find(current_char) != std::string::npos; 
	};

	while (current_char != '\0' && hasDot()) {
		if (current_char == '.') {
			
			if (dots == 1)
				break;

			++dots;
			str += '.';
		}
		else {
			str += current_char;
		}

		advance();
	}

	if (dots == 0) {
		return new Token(
			Token::Type::INT, 
			(int)std::atoi(str.c_str()), 
			start,
			cursor
		);
	}
	else {
		return new Token(
			Token::Type::DOUBLE,
			std::atof(str.c_str()),
			start, 
			cursor
		);
	}
}

Token* Lexer::create_identifier()
{
	std::string id;
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
	auto str = letters_digits + "_";

	auto isLetterOrDigit = [=]() { 
		return str.find(current_char) != std::string::npos;
	};

	while (current_char != '\0' && isLetterOrDigit()) {
		id += current_char;
		advance();
	}

	Token::Type type = Token::Type::NONE;

	std::vector<std::string>::iterator it = std::find(
		Token::keywords.begin(), 
		Token::keywords.end(), 
		id
	);

	type = it != Token::keywords.end()
		? Token::Type::KEYWORD
		: Token::Type::IDENTIFIER;

	return new Token(type, id, start, cursor);
}

Token* Lexer::create_equals_operator()
{
	Token::Type type = Token::Type::EQ;
	std::string value = "=";
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);

	advance();

	if (current_char == '=') {
		advance();
		type = Token::Type::EE;
		value = "==";
	}

	return new Token(type, value, start, cursor);
}

Token* Lexer::create_not_equals_operator()
{
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
	advance();

	if (current_char == '=') {
		advance();
		return new Token(Token::Type::NE, "!=", start, cursor);
	}

	advance();
	return nullptr;
}

Token* Lexer::create_less_operator()
{
	Token::Type type = Token::Type::LT;
	std::string value = "<";
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
	advance();

	if (current_char == '=') {
		advance();
		type = Token::Type::LTE;
		value = "<=";
	}

	return new Token(type, value, start, cursor);
}

Token* Lexer::create_greater_operator()
{
	Token::Type type = Token::Type::GT;
	std::string value = ">";
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
	advance();

	if (current_char == '=') {
		advance();
		type = Token::Type::GTE;
		value = ">=";
	}

	return new Token(type, value, start, cursor);
}

Token* Lexer::create_minus_arrow_operator()
{
	Token::Type type = Token::Type::MINUS;
	std::string value = "-";
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
	advance();

	if (current_char == '>') {
		advance();
		type = Token::Type::ARROW;
		value = "->";
	}

	return new Token(type, value, start, cursor);
}

Token* Lexer::create_string()
{
	std::string str;
	std::shared_ptr<Cursor> start = std::make_shared<Cursor>(*cursor);
	bool escape = false;
	advance();

	std::unordered_map<char, char> escape_chars = {
		{'n', '\n'},
		{'t', '\t'}
	};
	
	while (current_char != '\0' && (current_char != '"' || escape)) {
		if (escape) {
			auto it = escape_chars.find(current_char);

			str += it != escape_chars.end()
				? it->second
				: current_char;

			escape = false;
		}
		else {
			if (current_char == '\\')
				escape = true;
			else
				str += current_char;
		}
	
		advance();
	}

	advance();
	return new Token(Token::Type::STRING, str, start, cursor);
}
