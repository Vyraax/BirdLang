#pragma once

#include "Cursor.h"
#include "Error.h"
#include "Context.h"
#include "Platform.h"

class API Number {
public:
	Number(const std::variant<float, int>& value = 0);

	std::pair<Number*, Error*> add(Number* other);
	std::pair<Number*, Error*> subtract(Number* other);
	std::pair<Number*, Error*> multiply(Number* other);
	std::pair<Number*, Error*> divide(Number* other);
	std::pair<Number*, Error*> power(Number* other);
	std::pair<Number*, Error*> compare_equal(Number* other);
	std::pair<Number*, Error*> compare_not_equal(Number* other);
	std::pair<Number*, Error*> compare_less_than(Number* other);
	std::pair<Number*, Error*> compare_greater_than(Number* other);
	std::pair<Number*, Error*> compare_less_or_equal(Number* other);
	std::pair<Number*, Error*> compare_greater_or_equal(Number* other);
	std::pair<Number*, Error*> compare_and(Number* other);
	std::pair<Number*, Error*> compare_or(Number* other);
	std::pair<Number*, Error*> compare_not(Number* other);

	inline friend std::ostream& operator << (std::ostream& stream, Number* number) {

		if (number->value.index() == 0) {
			try { stream << std::to_string(std::get<float>(number->value)); }
			catch (const std::bad_variant_access&) {}
		}
		else {
			try { stream << std::to_string(std::get<int>(number->value)); }
			catch (const std::bad_variant_access&) {}
		}

		return stream;
	}

	std::variant<float, int> value;
	Cursor* start;
	Cursor* end;
	std::shared_ptr<Context> context;
};