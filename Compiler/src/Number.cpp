#include "pch.h"
#include "Number.h"

Number::Number(const std::variant<float, int, bool, Function*>& value) :
	Type(value)
{
}

std::pair<Number*, Error*> Number::add(Type* other)
{
    Number* result = new Number();
	result->context = context;
    
    if (value.index() == 0 && other->value.index() == 0) {
		try { result->value = std::get<double>(value) + std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
    }
    else if(value.index() == 1 && other->value.index() == 1) {
		try { result->value = std::get<int>(value) + std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
    }
	else if (value.index() == 0 && other->value.index() == 1) {
		try { result->value = std::get<double>(value) + (double)std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 0) {
		try { result->value = (double)std::get<int>(value) + std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}

    return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::subtract(Type* other)
{
	Number* result = new Number();
	result->context = context;

	if (value.index() == 0 && other->value.index() == 0) {
		try { result->value = std::get<double>(value) - std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 1) {
		try { result->value = std::get<int>(value) - std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 0 && other->value.index() == 1) {
		try { result->value = std::get<double>(value) - (double)std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 0) {
		try { result->value = (double)std::get<int>(value) - std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}

	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::multiply(Type* other)
{
	Number* result = new Number();
	result->context = context;

	if (value.index() == 0 && other->value.index() == 0) {
		try { result->value = std::get<double>(value) * std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 1) {
		try { result->value = std::get<int>(value) * std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 0 && other->value.index() == 1) {
		try { result->value = std::get<double>(value) * (double)std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 0) {
		try { result->value = (double)std::get<int>(value) * std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}

	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::modulus(Type* other)
{
	Number* result = new Number();
	result->context = context;

	if (value.index() == 1 && other->value.index() == 1) {
		try { result->value = std::get<int>(value) % std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else
		return std::make_pair(nullptr, new RuntimeError(other->start, other->end, "Modulus supports only integers.", context));

	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::divide(Type* other)
{
	Number* result = new Number();
	result->context = context;

	if (value.index() == 0 && other->value.index() == 0) {
		try { 
			auto rhs = std::get<double>(other->value);

			if (rhs == 0.0)
				return std::make_pair(nullptr, new RuntimeError(other->start, other->end, "Division by zero", context));
			else
				result->value = std::get<double>(value) / rhs;
		}
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 1) {
		try { 
			auto rhs = std::get<int>(other->value);

			if (rhs == 0)
				return std::make_pair(nullptr, new RuntimeError(other->start, other->end, "Division by zero", context));
			else
				result->value = (double)std::get<int>(value) / (double)rhs;
		}
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 0 && other->value.index() == 1) {
		try {
			auto rhs = std::get<int>(other->value);

			if (rhs == 0)
				return std::make_pair(nullptr, new RuntimeError(other->start, other->end, "Division by zero", context));
			else
				result->value = std::get<double>(value) / (double)rhs;
		}
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 0) {
		try {
			auto rhs = std::get<double>(other->value);

			if (rhs == 0)
				return std::make_pair(nullptr, new RuntimeError(other->start, other->end, "Division by zero", context));
			else
				result->value = (double)std::get<int>(value) / rhs;
		}
		catch (const std::bad_variant_access&) {}
	}

	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::power(Type* other)
{
	Number* result = new Number();
	result->context = context;

	if (value.index() == 0 && other->value.index() == 0) {
		try { result->value = (double)pow(std::get<double>(value), std::get<double>(other->value)); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 1) {
		try { result->value = (double)pow((double)std::get<int>(value), std::get<int>(other->value)); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 0 && other->value.index() == 1) {
		try { result->value = (double)pow(std::get<double>(value), (double)std::get<int>(other->value)); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 0) {
		try { result->value = (double)pow((double)std::get<int>(value), std::get<double>(other->value)); }
		catch (const std::bad_variant_access&) {}
	}

	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_equal(Type* other)
{
	Number* result = new Number();
	result->value = (bool)(value == other->value);
	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_not_equal(Type* other)
{
	Number* result = new Number();
	result->value = value != other->value;
	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_less_than(Type* other)
{
	Number* result = new Number();
	result->value = value < other->value;
	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_greater_than(Type* other)
{
	Number* result = new Number();
	result->value = value > other->value;
	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_less_or_equal(Type* other)
{
	Number* result = new Number();
	result->value = value <= other->value;
	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_greater_or_equal(Type* other)
{
	Number* result = new Number();
	result->value = value >= other->value;
	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_and(Type* other)
{
	Number* result = new Number();

	if (value.index() == 0 && other->value.index() == 0) {
		try { result->value = std::get<double>(value) && std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 1) {
		try { result->value = (double)std::get<int>(value) && std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 0 && other->value.index() == 1) {
		try { result->value = std::get<double>(value) && (double)std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 0) {
		try { result->value = (double)std::get<int>(value) && std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 2 && other->value.index() == 2) {
		try { result->value = (bool)std::get<bool>(value) && std::get<bool>(other->value); }
		catch (const std::bad_variant_access&) {}
	}

	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_or(Type* other)
{
	Number* result = new Number();

	if (value.index() == 0 && other->value.index() == 0) {
		try { result->value = std::get<double>(value) || std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 1) {
		try { result->value = (double)std::get<int>(value) || std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 0 && other->value.index() == 1) {
		try { result->value = std::get<double>(value) || (double)std::get<int>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1 && other->value.index() == 0) {
		try { result->value = (double)std::get<int>(value) ||  std::get<double>(other->value); }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 2 && other->value.index() == 2) {
		try { result->value = (bool)std::get<bool>(value) || std::get<bool>(other->value); }
		catch (const std::bad_variant_access&) {}
	}

	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::compare_not(Type* other)
{
	Number* result = new Number();

	if (value.index() == 0) {
		try { result->value = std::get<double>(value) == 0.0 ? 1 : 0; }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1) {
		try { result->value = std::get<int>(value) == 0 ? 1 : 0; }
		catch (const std::bad_variant_access&) {}
	}

	return std::make_pair(result, nullptr);
}

std::pair<Number*, Error*> Number::is_true()
{
	Number* result = new Number();

	if (value.index() == 0) {
		try { result->value = std::get<double>(value) != 0.0; }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 1) {
		try { result->value = std::get<int>(value) != 0; }
		catch (const std::bad_variant_access&) {}
	}
	else if (value.index() == 2) {
		try { result->value = std::get<bool>(value) != false; }
		catch (const std::bad_variant_access&) {}
	}

	return std::make_pair(result, nullptr);
}
