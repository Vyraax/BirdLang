#include "pch.h"
#include "Parser.h"

Parser::Parser() :
	current_token(nullptr),
	index(-1),
	debug(true)
{
}

Parser::Result* Parser::parse()
{
	advance();
	Result* result = expr();

	if (result != nullptr) {
		if (result->error == nullptr && current_token->type != Token::Type::EOT) {
			return result->failure(new InvalidSyntaxError(
				current_token->start,
				current_token->end,
				"Expected '+', '-', '*', '/', '^', '==', '!=', '<', '>', '<=', '>= 'and' or 'or'"
			));
		}

		if (debug) {
			traverse(result->node, 0);
		}
	}

	return result;
}

void Parser::traverse(Node* node, unsigned int depth)
{
	if (node == nullptr)
		return;
	
	++depth;

	traverse(node->left, depth);
	std::cout << std::string(depth, '\t') << node << std::endl;
	traverse(node->right, depth);
}

Token* Parser::advance()
{
	++index;

	if (index < tokens.size()) {
		current_token = tokens.at(index);
	}

	return current_token;
}

Parser::Result* Parser::factor()
{
	Result* result = new Result();

	if (current_token->type == Token::Type::PLUS ||
		current_token->type == Token::Type::MINUS) {
		Token* token = new Token(current_token);

		result->record_advance();
		advance();
		auto component = result->record(factor());

		if (result->error != nullptr)
			return result;

		return result->success(new UnaryOperationNode(component, token));
	}

	return power();
}

Parser::Result* Parser::term()
{
	return binary_operation([=]() {
		return factor();
	}, { 
		Token::Type::MUL,
		Token::Type::DIV
	}, {});
}

Parser::Result* Parser::arithm()
{
	return binary_operation([=]() {
		return term();
	}, {
		Token::Type::PLUS,
		Token::Type::MINUS
	}, {});
}

Parser::Result* Parser::power()
{
	return binary_operation([=]() {
		return atom();
	}, {
		Token::Type::POW
	}, {}, [=]() {
		return factor();
	});
}

Parser::Result* Parser::expr()
{
	if (current_token != nullptr) {
		Result* result = new Result();

		std::string value;
		try { value = std::get<std::string>(current_token->value); }
		catch (const std::bad_variant_access&) {}

		if (current_token->type == Token::Type::KEYWORD && value == "var") {
			result->record_advance();
			advance();

			if (current_token->type != Token::Type::IDENTIFIER) {
				return result->failure(new InvalidSyntaxError(
					current_token->start,
					current_token->end,
					"Expected Identifier"
				));
			}

			Token* var_name = new Token(current_token);
			result->record_advance();
			advance();

			if (current_token->type != Token::Type::EQ) {
				return result->failure(new InvalidSyntaxError(
					current_token->start,
					current_token->end,
					"Expected '='"
				));
			}

			result->record_advance();
			advance();

			Node* expression = result->record(expr());

			if (result->error != nullptr) {
				return result;
			}

			return result->success(new VariableAssignmentNode(var_name, expression));
		}

		auto node = result->record(binary_operation([=]() {
			return compare();
		}, {
			Token::Type::KEYWORD,
			Token::Type::KEYWORD
		}, { "and", "or" }));

		if (result->error != nullptr) {
			return result->failure(new InvalidSyntaxError(
				current_token->start,
				current_token->end,
				"Expected 'var', int, float, identifier, '+', '-', '(' or 'not'"
			));
		}

		return result->success(node);
	}
	
	return nullptr;
}

Parser::Result* Parser::if_expr()
{
	if (current_token != nullptr) {
		Result* result = new Result();
		Node* else_expr = nullptr;
		Node* condition = nullptr;
		std::vector<std::pair<Node*, Node*>> cases;

		std::string if_value;
		try { if_value = std::get<std::string>(current_token->value); }
		catch (const std::bad_variant_access&) {}

		if (if_value != "if") {
			return result->failure(new InvalidSyntaxError(
				current_token->start,
				current_token->end,
				"Expected 'if'"
			));
		}

		result->record_advance();
		advance();

		condition = result->record(expr());

		if (result->error != nullptr)
			return result;

		std::cout << current_token << std::endl;

		std::string then_value;
		try { then_value = std::get<std::string>(current_token->value); }
		catch (const std::bad_variant_access&) {}

		if (then_value != "then") {
			return result->failure(new InvalidSyntaxError(
				current_token->start,
				current_token->end,
				"Expected 'then'"
			));
		}

		result->record_advance();
		advance();

		auto exp = result->record(expr());

		if (result->error != nullptr)
			return result;

		cases.push_back(std::make_pair(condition, exp));

		auto is_elsif = [=]() {
			std::string val;
			try { val = std::get<std::string>(current_token->value); }
			catch (const std::bad_variant_access&) {}

			return val == "elseif";
		};

		while (current_token->type == Token::Type::KEYWORD && is_elsif()) {
			result->record_advance();
			advance();

			condition = result->record(expr());

			if (result->error != nullptr)
				return result;

			std::string elseif_then;
			try { elseif_then = std::get<std::string>(current_token->value); }
			catch (const std::bad_variant_access&) {}

			if (current_token->type != Token::Type::KEYWORD || elseif_then != "then") {
				return result->failure(new InvalidSyntaxError(
					current_token->start,
					current_token->end,
					"Expected 'then'"
				));
			}

			result->record_advance();
			advance();

			auto then_expr = result->record(expr());

			if (result->error != nullptr)
				return result;

			cases.push_back(std::make_pair(condition, then_expr));
		}

		std::string else_value;
		try { else_value = std::get<std::string>(current_token->value); }
		catch (const std::bad_variant_access&) {}

		if (current_token->type == Token::Type::KEYWORD && else_value != "else") {
			result->record_advance();
			advance();

			else_expr = result->record(expr());

			if (result->error != nullptr)
				return result;
		}

		Token* token = new Token(current_token);

		return result->success(new IfStatementNode(token, cases, else_expr));
	}
	
	return nullptr;
}

Parser::Result* Parser::atom()
{
	if (current_token != nullptr) {

		Result* result = new Result();

		if (current_token->type == Token::Type::FLOAT ||
			current_token->type == Token::Type::INT) {
			Token* token = new Token(current_token);

			result->record_advance();
			advance();

			return result->success(new NumericNode(token));
		}
		else if (current_token->type == Token::Type::IDENTIFIER) {
			Token* token = new Token(current_token);

			result->record_advance();
			advance();

			return result->success(new VariableAccessNode(token));
		}
		else if (current_token->type == Token::Type::LPAREN) {
			Token* token = new Token(current_token);

			result->record_advance();
			advance();

			auto exp = result->record(expr());

			if (result->error != nullptr)
				return result;

			if (current_token->type == Token::Type::RPAREN) {
				result->record_advance();
				advance();

				return result->success(exp);
			}

			return result->failure(new InvalidSyntaxError(
				current_token->start,
				current_token->end,
				"Expected ')'"
			));
		}
		else {
			std::string value;
			try { value = std::get<std::string>(current_token->value); }
			catch (const std::bad_variant_access&) {}

			if (current_token->type == Token::Type::KEYWORD && value == "if") {

				auto exp = result->record(if_expr());

				if (result->error != nullptr)
					return result;

				return result->success(exp);
			}
		}

		return result->failure(new InvalidSyntaxError(
			current_token->start,
			current_token->end,
			"Expected Integer, Float, '+', '-' or '('"
		));
	}

	return nullptr;
}

Parser::Result* Parser::compare()
{
	Result* result = new Result();

	std::string value;
	try { value = std::get<std::string>(current_token->value); }
	catch (const std::bad_variant_access&) {}

	if (current_token->type == Token::Type::KEYWORD && value == "not") {

		auto token = new Token(current_token);

		result->record_advance();
		advance();

		auto node = result->record(compare());

		if (result->error != nullptr)
			return result;

		return result->success(new UnaryOperationNode(node, token));
	}

	auto node = result->record(binary_operation([=]() {
		return arithm();
	}, {
		Token::Type::EE,
		Token::Type::NE,
		Token::Type::LT,
		Token::Type::GT,
		Token::Type::LTE,
		Token::Type::GTE
	}, {}));

	if (result->error != nullptr) {
		return result->failure(new InvalidSyntaxError(
			current_token->start,
			current_token->end,
			"Expected Integer, Float, '+', '-', '(', 'not' "
		));
	}

	return result->success(node);
}

Parser::Result* Parser::binary_operation(
	std::function<Result*()> fna, 
	const std::vector<Token::Type>& operations, 
	const std::vector<std::string>& values, 
	std::function<Result*()> fnb
)
{
	if (current_token != nullptr) {

		if (fnb == nullptr)
			fnb = fna;

		Result* result = new Result();
		Node* left = result->record(fna());
		
		if (result->error != nullptr)
			return result;

		auto type_exists = [=]() {
			return std::find(operations.begin(), operations.end(), current_token->type) != operations.end();
		};

		auto value_exists = [=]() {
			std::string value;
			try { value = std::get<std::string>(current_token->value); }
			catch (const std::bad_variant_access&) {}

			return std::find(values.begin(), values.end(), value) != values.end() && type_exists();
		};

		while (type_exists() || value_exists()) {
			Token* token = new Token(current_token);

			result->record_advance();
			advance();
			Node* right = result->record(fnb());

			if (result->error != nullptr)
				return result;

			left = new BinaryOperationNode(left, token, right);
		}

		return result->success(left);
	}

	return nullptr;
}
