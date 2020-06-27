#pragma once

#include "Cursor.h"
#include "Platform.h"

#include <vector>
#include <variant>

class API Token
{
public:
	enum class Type {
		INT,
		FLOAT,
		STRING,
		PLUS,
		MINUS,
		POW,
		MUL,
		DIV,
		LPAREN,
		RPAREN,
		IDENTIFIER,
		KEYWORD,
		EQ,
		EE,
		NE,
		LT,
		GT,
		LTE,
		GTE,
		EOT,
		NONE
	};

	static std::vector<std::string> keywords;

	Token(Token* token) {
		type = token->type;
		value = token->value;
		start = token->start;
		end = token->end;
	}

	Token(const Type& type = Type::NONE, const std::variant<float, int, char, std::string>& value = '0', Cursor* start = nullptr, Cursor* end = nullptr);

	static inline std::string toString(Type type) {
		switch (type) {
		case Type::INT: return "INT";
		case Type::FLOAT: return "FLOAT";
		case Type::STRING: return "STRING";
		case Type::PLUS: return "PLUS";
		case Type::MINUS: return "MINUS";
		case Type::POW: return "POW";
		case Type::MUL: return "MUL";
		case Type::DIV: return "DIV";
		case Type::LPAREN: return "LPAREN";
		case Type::RPAREN: return "RPAREN";
		case Type::IDENTIFIER: return "IDENTIFIER";
		case Type::KEYWORD: return "KEYWORD";
		case Type::EQ: return "EQ";
		case Type::EE: return "EE";
		case Type::NE: return "NE";
		case Type::LT: return "LT";
		case Type::GT: return "GT";
		case Type::LTE: return "LTE";
		case Type::GTE: return "GTE";
		case Type::EOT: return "EOT";
		case Type::NONE: return "NONE";
		}

		return "UNDEFINED";
	}

	inline friend std::ostream& operator << (std::ostream& stream, Token* token) {
		auto token_type = toString(token->type) + ": ";
		stream << token_type;

		switch (token->value.index()) {
		case 0:
			try {
				stream << std::get<float>(token->value);
			}
			catch (const std::bad_variant_access&) {}
			break;
		case 1:
			try {
				stream << std::get<int>(token->value);
			}
			catch (const std::bad_variant_access&) {}
			break;
		case 2:
			try {
				stream << std::get<char>(token->value);
			}
			catch (const std::bad_variant_access&) {}
			break;
		case 3:
			try {
				stream << std::get<std::string>(token->value);
			}
			catch (const std::bad_variant_access&) {}
			break;
		}

		return stream;
	}

	Type type;
	Cursor* start;
	Cursor* end;
	std::variant<float, int, char, std::string> value;
};