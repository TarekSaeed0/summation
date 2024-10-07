#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <assert.h>
#include <ctype.h>
#include <environment.h>
#include <stddef.h>

/**
 * @brief a mathematical expression.
 *
 * This data structure represents a mathematical expression that might contain variables.
 * variables are represented with a single alphabet letter.
 *
 * Expression grammar
 * ------------------
 * * atom = number | identifier, [ "(", [ expression ], { ",", expression }, ")" ] | "(" expression
 * ")"
 * * primary = atom, [ "^", factor ]
 * * factor = "-" factor | primary
 * * term = factor, { ("*" | "/"), factor }
 * * expression = term, { ("+" | "-"), term }
 */
struct expression {
	enum expression_type {
		expression_type_constant,
		expression_type_variable,
		expression_type_operation,
	} type;
	union {
		struct constant {
			double value;
		} constant;
		struct variable {
			char name;
		} variable;
		struct operation {
			enum operation_type {
				operation_type_addition,
				operation_type_subtraction,
				operation_type_multiplication,
				operation_type_division,
				operation_type_exponentiation,
				operation_type_negation,
				operation_type_sine,
				operation_type_cosine,
				operation_type_tangent,
				operation_type_exponential,
				operation_type_logarithm,
			} type;
			struct expression *operands;
		} operation;
	};
};

/**
 * @brief Gets the arity of an operation.
 *
 * Returns the number of operands that an operation of type `type` takes.
 *
 * @param[in] type The type of the operation.
 * @return The arity of the operation.
 *
 * @memberof operation_type
 */
static inline size_t operation_type_arity(enum operation_type type) {
	return 1 + (type <= operation_type_exponentiation);
}

/**
 * @brief Gets the precedence of an operation.
 *
 * Returns the priority of an operation of type `type`.
 *
 * @param[in] type The type of the operation.
 * @return The precedence of the operation.
 *
 * @memberof operation_type
 */
static inline size_t operation_type_precedence(enum operation_type type) {
	switch (type) {
		case operation_type_addition:
		case operation_type_subtraction: return 0;
		case operation_type_multiplication:
		case operation_type_division: return 1;
		case operation_type_exponentiation:
		case operation_type_negation: return 2;
		case operation_type_sine:
		case operation_type_cosine:
		case operation_type_tangent:
		case operation_type_exponential:
		case operation_type_logarithm: return 3;
	}
}

/**
 * @brief Creates a new expression of type constant.
 *
 * Returns a new expression of type constant with the given value.
 *
 * @param[in] value The constant's value.
 * @return The newly created expression.
 *
 * @memberof expression
 */
static inline struct expression expression_constant(double value) {
	return (struct expression){
		.type = expression_type_constant,
		.constant = { .value = value },
	};
}

/**
 * @brief Creates a new expression of type variable
 *
 * Returns a new expression of type variable with the given name.
 *
 * @param[in] name The variable's name, must be an alphabet letter.
 * @return The newly created expression.
 *
 * @memberof expression
 */
static inline struct expression expression_variable(char name) {
	assert(isalpha((unsigned char)name));

	return (struct expression){
		.type = expression_type_variable,
		.variable = { .name = name },
	};
}

/**
 * @brief Creates a new expression of type operation.
 *
 * Returns a new expression of type operation with the given type and operands.
 * The number of operands must match the arity of the operation.
 *
 * @param[in] type The operation's type.
 * @return The newly created expression.
 *
 * @memberof expression
 */
struct expression expression_operation(enum operation_type type, ...);

/**
 * @brief Clones an expression
 *
 * Returns a deep copy of `expression`
 *
 * @param[in] expression The expression to be cloned
 * @return The newly created clone.
 *
 * @memberof expression
 */
struct expression expression_clone(struct expression expression);

/**
 * @brief Drops an expression.
 *
 * Releases all memory and resources owned by the expression
 *
 * @param[in] expression The expression to drop.
 *
 * @memberof expression
 */
void expression_drop(struct expression expression);

/**
 * @brief Creates an expression from a string.
 *
 * Parses the given string into an expression.
 *
 * @param[in] string The string to be parsed.
 * @return The newly created expression.
 *
 * @memberof expression
 */
struct expression expression_from_string(const char *string);

/**
 * @brief Converts an expression to a string.
 *
 * Creates a human-readable string representation of the given expression.
 * The returned string must be freed with `free()`
 *
 * @param[in] expression The expression to be converted.
 * @return The newly created string.
 *
 * @memberof expression
 */
char *expression_to_string(struct expression expression);

/**
 * @brief Prints an expression in debug format.
 *
 * Prints the expression in a format that is suitable for debugging.
 *
 * @param[in] expression The expression to be printed.
 *
 * @memberof expression
 */
void expression_debug_print(struct expression expression);

/**
 * @brief Evaluates an expression
 *
 * Returns the result of evaluating the given expression in the given environment
 *
 * @param[in] expression The expression to be evaluated.
 * @param[in] environment The environment the expression is evaluated in.
 * @return the result of the expression
 *
 * @memberof expression
 */
double expression_evaluate(struct expression expression, const struct environment *environment);

#endif
