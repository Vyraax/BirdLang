#include "pch.h"
#include "BaseFunction.h"
#include "Error.h"
#include "Interpreter.h"

BaseFunction::BaseFunction(
	const std::string& name,
	Node* body,
	const std::vector<std::string>& args_names,
	std::shared_ptr<Cursor> start,
	std::shared_ptr<Cursor> end,
	Context* context
) :
	Type(),
	body(body),
	args_names(args_names)
{
	this->name = name.size() == 0
		? "<anonymous>"
		: name;
}

Context* BaseFunction::generate_context()
{
	auto ctx = new Context(name, context, this->start);
	ctx->symbols = new Symbols(ctx->parent->symbols);

	return ctx;
}

RuntimeResult* BaseFunction::check_arguments(
	const std::vector<std::string>& names,
	const std::vector<Type*>& args
)
{
	RuntimeResult* result = new RuntimeResult();

	if (args.size() > names.size()) {
		return result->failure(new RuntimeError(
			this->start,
			this->end,
			std::to_string(args.size() - args_names.size())
			+ " too many args passed into '" + name + '\'',
			context
		));
	}

	if (args.size() < names.size()) {
		return result->failure(new RuntimeError(
			this->start,
			this->end,
			std::to_string(names.size() - args.size())
			+ " too few args passed into '" + name + '\'',
			context
		));
	}

	return result->success(nullptr);
}

void BaseFunction::populate_arguments(
	const std::vector<std::string>& names,
	const std::vector<Type*>& args,
	Context* ctx
)
{
	for (unsigned int i = 0; i < args.size(); ++i) {
		auto name = names.at(i);
		auto value = args.at(i);
		value->context = ctx;
		ctx->symbols->set(name, value->value);
	}
}

RuntimeResult* BaseFunction::check_and_populate_arguments(
	const std::vector<std::string>& names,
	const std::vector<Type*>& args,
	Context* ctx
)
{
	RuntimeResult* result = new RuntimeResult();
	result->record(check_arguments(names, args));

	if (result->error != nullptr)
		return result;

	populate_arguments(names, args, ctx);

	return result->success(nullptr);
}