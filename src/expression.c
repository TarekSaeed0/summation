#include <expression.h>

#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct expression expression_operation(enum operation_type type, ...) {
	va_list arguments;
	va_start(arguments, type);

	size_t arity = operation_type_arity(type);

	struct expression *operands = malloc(arity * sizeof(*operands));
	for (size_t i = 0; i < arity; i++) {
		operands[i] = va_arg(arguments, struct expression);
	}

	va_end(arguments);

	return (struct expression){
		.type = expression_type_operation,
		.operation = { .type = type, .operands = operands },
	};
}

struct expression expression_clone(struct expression expression) {
	switch (expression.type) {
		case expression_type_constant:
		case expression_type_variable: return expression;
		case expression_type_operation: {
			size_t arity = operation_type_arity(expression.operation.type);
			struct expression *operands = malloc(arity * sizeof(*operands));
			for (size_t i = 0; i < arity; i++) {
				operands[i] = expression_clone(expression.operation.operands[i]);
			}
			return (struct expression){
				.type = expression_type_operation,
				.operation = { .type = expression.operation.type, .operands = operands },
			};
		} break;
	}
}

void expression_drop(struct expression expression) {
	switch (expression.type) {
		case expression_type_constant:
		case expression_type_variable: break;
		case expression_type_operation: {
			size_t arity = operation_type_arity(expression.operation.type);
			for (size_t i = 0; i < arity; i++) {
				expression_drop(expression.operation.operands[i]);
			}
			free(expression.operation.operands);
		} break;
	}
}

static struct expression expression_from_string_expression(const char **string);
static struct expression expression_from_string_atom(const char **string) {
	assert(string != NULL && *string != NULL);

	struct expression atom;

	// skip whitespace
	while (isspace(**string)) {
		++*string;
	}

	if (**string == '(') {
		++*string;

		atom = expression_from_string_expression(string);

		// skip whitespace
		while (isspace(**string)) {
			++*string;
		}

		if (**string == ')') {
			++*string;
		} else {
			(void)fprintf(stderr, "Warning: unclosed parentheses \"%s\"\n", *string);
		}
	} else if (isalpha(**string)) {
		size_t length = 1;
		while (isalpha((*string)[length])) {
			++length;
		}
		struct {
			const char *name;
			enum operation_type type;
		} functions[] = {
			{ "sin", operation_type_sine },		 { "cos", operation_type_cosine },
			{ "tan", operation_type_tangent },	 { "exp", operation_type_exponential },
			{ "log", operation_type_logarithm },
		};

		bool is_function = false;

		for (size_t i = 0; i < sizeof(functions) / sizeof(functions[0]); i++) {
			if (strncmp(functions[i].name, *string, length) == 0) {
				is_function = true;

				*string += length;

				atom = expression_operation(
					functions[i].type,
					expression_from_string_expression(string)
				);

				break;
			}
		}

		if (!is_function) {
			char name = **string;

			++*string;

			atom = expression_variable(name);
		}
	} else {
		char *end = NULL;
		double value = strtod(*string, &end);
		if (end == *string) {
			(void)fprintf(stderr, "Error: failed to parse constant from \"%s\"\n", *string);
			value = NAN;
		}

		if (errno == ERANGE) {
			(void
			)fprintf(stderr, "Warning: constant parsed from \"%s\" is out of range\n", *string);
		}

		*string = end;

		atom = expression_constant(value);
	}

	return atom;
}
static struct expression expression_from_string_factor(const char **string);
static struct expression expression_from_string_primary(const char **string) {
	assert(string != NULL && *string != NULL);

	struct expression primary = expression_from_string_atom(string);

	// skip whitespace
	while (isspace(**string)) {
		++*string;
	}

	if (**string == '^') {
		++*string;

		primary = expression_operation(
			operation_type_exponentiation,
			primary,
			expression_from_string_factor(string)
		);
	}

	return primary;
}
static struct expression expression_from_string_factor(const char **string) {
	assert(string != NULL && *string != NULL);

	struct expression factor;

	// skip whitespace
	while (isspace(**string)) {
		++*string;
	}

	if (**string == '-') {
		++*string;

		factor =
			expression_operation(operation_type_negation, expression_from_string_factor(string));
	} else {
		factor = expression_from_string_primary(string);
	}

	return factor;
}
static struct expression expression_from_string_term(const char **string) {
	assert(string != NULL && *string != NULL);

	struct expression expression = expression_from_string_factor(string);

	while (1) {
		// skip whitespace
		while (isspace(**string)) {
			++*string;
		}

		switch (**string) {
			case '*': {
				++*string;

				expression = expression_operation(
					operation_type_multiplication,
					expression,
					expression_from_string_factor(string)
				);
			} break;
			case '/': {
				++*string;

				expression = expression_operation(
					operation_type_division,
					expression,
					expression_from_string_factor(string)
				);
			} break;
			default: return expression;
		}
	}
}
static struct expression expression_from_string_expression(const char **string) {
	assert(string != NULL && *string != NULL);

	struct expression expression = expression_from_string_term(string);

	while (1) {
		// skip whitespace
		while (isspace(**string)) {
			++*string;
		}

		switch (**string) {
			case '+': {
				++*string;

				expression = expression_operation(
					operation_type_addition,
					expression,
					expression_from_string_term(string)
				);
			} break;
			case '-': {
				++*string;

				expression = expression_operation(
					operation_type_subtraction,
					expression,
					expression_from_string_term(string)
				);
			} break;
			default: return expression;
		}
	}
}

struct expression expression_from_string(const char *string) {
	assert(string != NULL);

	return expression_from_string_expression(&string);
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
static int expression_to_string_(
	char *string,
	size_t maximum_length,
	struct expression expression
) {
	int total_length = 0;
#define print(function, ...)                                                                       \
	do {                                                                                           \
		int length = function(string, maximum_length, __VA_ARGS__);                                \
		if (length < 0) {                                                                          \
			return length;                                                                         \
		}                                                                                          \
		if ((size_t)length >= maximum_length) {                                                    \
			string += maximum_length;                                                              \
			maximum_length = 0;                                                                    \
		} else {                                                                                   \
			string += length;                                                                      \
			maximum_length -= (size_t)length;                                                      \
		}                                                                                          \
		total_length += length;                                                                    \
	} while (0)

	switch (expression.type) {
		case expression_type_constant: print(snprintf, "%lg", expression.constant.value); break;
		case expression_type_variable: print(snprintf, "%c", expression.variable.name); break;
		case expression_type_operation: {
			switch (expression.operation.type) {
				case operation_type_addition:
				case operation_type_subtraction:
				case operation_type_multiplication:
				case operation_type_division: {
					if (expression.operation.operands[0].type == expression_type_operation &&
						(operation_type_precedence(expression.operation.operands[0].operation.type
						 ) < operation_type_precedence(expression.operation.type))) {
						print(snprintf, "(");
						print(expression_to_string_, expression.operation.operands[0]);
						print(snprintf, ")");
					} else {
						print(expression_to_string_, expression.operation.operands[0]);
					}

					switch (expression.operation.type) {
						case operation_type_addition: print(snprintf, " + "); break;
						case operation_type_subtraction: print(snprintf, " - "); break;
						case operation_type_multiplication: print(snprintf, " * "); break;
						case operation_type_division: print(snprintf, " / "); break;
						// we have already checked the operation's type before
						default: __builtin_unreachable();
					}

					if (expression.operation.operands[1].type == expression_type_operation &&
						(operation_type_precedence(expression.operation.operands[1].operation.type
						 ) <= operation_type_precedence(expression.operation.type))) {
						print(snprintf, "(");
						print(expression_to_string_, expression.operation.operands[1]);
						print(snprintf, ")");
					} else {
						print(expression_to_string_, expression.operation.operands[1]);
					}
				} break;
				case operation_type_exponentiation: {
					if (expression.operation.operands[0].type == expression_type_operation &&
						(operation_type_precedence(expression.operation.operands[0].operation.type
						 ) <= operation_type_precedence(expression.operation.type))) {
						print(snprintf, "(");
						print(expression_to_string_, expression.operation.operands[0]);
						print(snprintf, ")");
					} else {
						print(expression_to_string_, expression.operation.operands[0]);
					}

					print(snprintf, " ^ ");

					if (expression.operation.operands[1].type == expression_type_operation &&
						(operation_type_precedence(expression.operation.operands[1].operation.type
						 ) < operation_type_precedence(expression.operation.type))) {
						print(snprintf, "(");
						print(expression_to_string_, expression.operation.operands[1]);
						print(snprintf, ")");
					} else {
						print(expression_to_string_, expression.operation.operands[1]);
					}
				} break;
				case operation_type_negation: {
					print(snprintf, "-");

					if (expression.operation.operands[0].type == expression_type_operation &&
						(operation_type_precedence(expression.operation.operands[0].operation.type
						 ) < operation_type_precedence(expression.operation.type))) {
						print(snprintf, "(");
						print(expression_to_string_, expression.operation.operands[0]);
						print(snprintf, ")");
					} else {
						print(expression_to_string_, expression.operation.operands[0]);
					}
				} break;
				case operation_type_sine:
				case operation_type_cosine:
				case operation_type_tangent:
				case operation_type_exponential:
				case operation_type_logarithm: {
					switch (expression.operation.type) {

						case operation_type_sine: print(snprintf, "sin"); break;
						case operation_type_cosine: print(snprintf, "cos"); break;
						case operation_type_tangent: print(snprintf, "tan"); break;
						case operation_type_exponential: print(snprintf, "exp"); break;
						case operation_type_logarithm: print(snprintf, "log"); break;
						// we have already checked the operation's type before
						default: __builtin_unreachable();
					}
					print(snprintf, "(");
					print(expression_to_string_, expression.operation.operands[0]);
					print(snprintf, ")");
				} break;
			}
		}
	}

	return total_length;
#undef print
}

char *expression_to_string(struct expression expression) {
	int length = expression_to_string_(NULL, 0, expression);
	if (length < 0) {
		return NULL;
	}

	char *string = malloc((size_t)length + 1);
	if (string == NULL) {
		return NULL;
	}

	if (expression_to_string_(string, (size_t)length + 1, expression) < 0) {
		free(string);
		return NULL;
	}

	return string;
}

void expression_debug_print(struct expression expression) {
	switch (expression.type) {
		case expression_type_constant: printf("constant(%lg)", expression.constant.value); break;
		case expression_type_variable: printf("variable(%c)", expression.variable.name); break;
		case expression_type_operation: {
			printf("operation(");
			switch (expression.operation.type) {
				case operation_type_addition: printf("addition("); break;
				case operation_type_subtraction: printf("subtraction("); break;
				case operation_type_multiplication: printf("multiplication("); break;
				case operation_type_division: printf("division("); break;
				case operation_type_exponentiation: printf("exponentiation("); break;
				case operation_type_negation: printf("negation("); break;
				case operation_type_sine: printf("sine("); break;
				case operation_type_cosine: printf("cosine("); break;
				case operation_type_tangent: printf("tangent("); break;
				case operation_type_exponential: printf("exponential("); break;
				case operation_type_logarithm: printf("logarithm("); break;
			}
			size_t arity = operation_type_arity(expression.operation.type);
			for (size_t i = 0; i < arity; i++) {
				if (i != 0) {
					printf(", ");
				}
				expression_debug_print(expression.operation.operands[i]);
			}
			printf("))");
		} break;
	}
}

double expression_evaluate(
	struct expression expression,
	struct expression_environment *environment
) {
	switch (expression.type) {
		case expression_type_constant: return expression.constant.value;
		case expression_type_variable:
			return environment->variables[(size_t)expression.variable.name - 'a'];
		case expression_type_operation: {
			switch (expression.operation.type) {
				case operation_type_addition:
					return expression_evaluate(expression.operation.operands[0], environment) +
						   expression_evaluate(expression.operation.operands[1], environment);
				case operation_type_subtraction:
					return expression_evaluate(expression.operation.operands[0], environment) -
						   expression_evaluate(expression.operation.operands[1], environment);
				case operation_type_multiplication:
					return expression_evaluate(expression.operation.operands[0], environment) *
						   expression_evaluate(expression.operation.operands[1], environment);
				case operation_type_division:
					return expression_evaluate(expression.operation.operands[0], environment) /
						   expression_evaluate(expression.operation.operands[1], environment);
				case operation_type_exponentiation:
					return pow(
						expression_evaluate(expression.operation.operands[0], environment),
						expression_evaluate(expression.operation.operands[1], environment)
					);
				case operation_type_negation:
					return -expression_evaluate(expression.operation.operands[0], environment);
				case operation_type_sine:
					return sin(expression_evaluate(expression.operation.operands[0], environment));
				case operation_type_cosine:
					return cos(expression_evaluate(expression.operation.operands[0], environment));
				case operation_type_tangent:
					return tan(expression_evaluate(expression.operation.operands[0], environment));
				case operation_type_exponential:
					return exp(expression_evaluate(expression.operation.operands[0], environment));
				case operation_type_logarithm:
					return log(expression_evaluate(expression.operation.operands[0], environment));
			}
		}
	}
}
