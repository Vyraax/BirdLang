#include "pch.h"

#include "Compiler.h"
#include "Profiler.h"
#include "Utils.h"
#include "ConsoleTable.h"
#include "NativeFunction.h"

using ConsoleTable = samilton::ConsoleTable;

Compiler::Compiler(
	bool debug_lexer,
	bool debug_parser,
	bool profiling
) :
	debug_lexer(debug_lexer),
	debug_parser(debug_parser),
	profiling(profiling),
	interpreting_time(0.0)
{
	context = new Context("<program>");
	symbols = new Symbols();

	symbols->set("null", Type::Null);
	symbols->set("true", Type::True);
	symbols->set("false", Type::False);

	symbols->set("PI", PI);
	symbols->set("TAU", TAU);
	symbols->set("PHI", 1.618033988749895);
	symbols->set("EULER", 2.718281828459045);
	symbols->set("SQRT1_2", 0.7071067811865476);
	symbols->set("SQRT2", 1.4142135623730951);

	auto it = NativeFunction::list.begin();

	for (; it != NativeFunction::list.end(); ++it) {
		NativeFunction* fn = new NativeFunction(it->first, nullptr, std::get<0>(it->second));
		symbols->set(it->first, (Function*)fn);
	}

	context->symbols = symbols;

	lexer = std::make_unique<Lexer>("<stdin>");
	lexer->debug = debug_lexer;

	parser = std::make_unique<Parser>();
	parser->debug = debug_parser;

	interpreter = std::make_unique<Interpreter>();
	std::cout.precision(std::numeric_limits<double>::max_digits10);
}

void Compiler::interpret(const std::string& input)
{
	auto tokens = lexer->index_tokens(input);
	parser->setTokens(tokens);
	auto ast = parser->parse();

	Profiler profiler;

	if (ast != nullptr) {
		if (ast->error != nullptr) {
			std::cout << ast->error << '\n';
		}
		else {
			if (profiling) {
				profiler.start = clock();
			}

			auto result = interpreter->visit(ast->node, context);

			if (profiling) {
				profiler.end = clock();
				interpreting_time = profiler.getReport();
				printStatistics();
			}

			if (result->error != nullptr) {
				if (strcmp(typeid(*result->error).name(), "class RuntimeError") == 0) {
					RuntimeError* error = static_cast<RuntimeError*>(result->error);

					if (error != nullptr) {
						std::cout << error << '\n';
					}
				}
				else {
					std::cout << result->error << '\n';
				}
			}
			else {
				if (result->value != nullptr) {
					std::cout << result->value << '\n';
				}
			}
		}
	}
}

void Compiler::printStatistics()
{
	std::cout << '\n';
	Utils::title("TIMING", 15, false);

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

	table[0][0] = "Step";
	table[0][1] = "Time";

	table[1][0] = "Tokenization";
	table[1][1] = lexer->lexing_time;

	table[2][0] = "Parsing";
	table[2][1] = parser->parsing_time;

	table[3][0] = "Evaluation";
	table[3][1] = interpreting_time;

	table[4][0] = "TOTAL";
	table[4][1] = lexer->lexing_time + interpreting_time + parser->parsing_time;

	std::cout << table << '\n';
}
