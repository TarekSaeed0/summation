#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include <expression.h>
#include <stdlib.h>
#include <string.h>

static void _assert_expression_equal(
	const struct expression *expression_1,
	const struct expression *expression_2,
	const char *const file,
	const int line
) {
	const bool is_equal = expression_equals(expression_1, expression_2);

	if (!is_equal) {
		char *string_1 = expression_to_string(expression_1);
		char *string_2 = expression_to_string(expression_2);

		print_error("%s != %s\n", string_1, string_2);

		free(string_1);
		free(string_2);

		_fail(file, line);
	}
}

#define assert_expression_equal(a, b) _assert_expression_equal(a, b, __FILE__, __LINE__)

struct test_state {
	size_t count;
	struct test_case {
		const char *string;
		struct expression expression;
	} test_cases[];
};

static int setup(void **state_) {
	struct test_case test_cases[] = {
		{ "1 + 2",
		  expression_operation(
			  operation_type_addition,
			  expression_constant(1),
			  expression_constant(2)
		  ) },
		{ "x / 2",
		  expression_operation(
			  operation_type_division,
			  expression_variable('x'),
			  expression_constant(2)
		  ) },
		{ "2 ^ 3 ^ 4 ^ 5",
		  expression_operation(
			  operation_type_exponentiation,
			  expression_constant(2),
			  expression_operation(
				  operation_type_exponentiation,
				  expression_constant(3),
				  expression_operation(
					  operation_type_exponentiation,
					  expression_constant(4),
					  expression_constant(5)
				  )
			  )
		  ) },
		{ "(0.23 + 3.5) * (2 - 1) ^ 2",
		  expression_operation(
			  operation_type_multiplication,
			  expression_operation(
				  operation_type_addition,
				  expression_constant(0.23),
				  expression_constant(3.5)
			  ),
			  expression_operation(
				  operation_type_exponentiation,
				  expression_operation(
					  operation_type_subtraction,
					  expression_constant(2),
					  expression_constant(1)
				  ),
				  expression_constant(2)
			  )
		  ) },
		{ "(2 ^ x) ^ 2",
		  expression_operation(
			  operation_type_exponentiation,
			  expression_operation(
				  operation_type_exponentiation,
				  expression_constant(2),
				  expression_variable('x')
			  ),
			  expression_constant(2)
		  ) },
		{ "sin(1 / x)",

		  expression_operation(
			  operation_type_sine,
			  expression_operation(
				  operation_type_division,
				  expression_constant(1),
				  expression_variable('x')
			  )
		  ) },
		{ "exp(5.2 * x - 2)",
		  expression_operation(
			  operation_type_exponential,
			  expression_operation(
				  operation_type_subtraction,
				  expression_operation(
					  operation_type_multiplication,
					  expression_constant(5.2),
					  expression_variable('x')
				  ),
				  expression_constant(2)
			  )
		  ) },
		{ "log(8 / x + sin(3.9))",
		  expression_operation(
			  operation_type_logarithm,
			  expression_operation(
				  operation_type_addition,
				  expression_operation(
					  operation_type_division,
					  expression_constant(8),
					  expression_variable('x')
				  ),
				  expression_operation(operation_type_sine, expression_constant(3.9))
			  )
		  ) },
	};

	struct test_state *state = malloc(sizeof(struct test_state) + sizeof(test_cases));
	assert_non_null(state);

	state->count = sizeof(test_cases) / sizeof(test_cases[0]);
	memcpy(state->test_cases, test_cases, sizeof(test_cases));

	*state_ = state;

	return 0;
}

static int teardown(void **state_) {
	struct test_state *state = *state_;

	for (size_t i = 0; i < state->count; i++) {
		expression_drop(&state->test_cases[i].expression);
	}
	free(state);

	return 0;
}

static void test_expression_equals(void **state_) {
	struct test_state *state = *state_;

	for (size_t i = 0; i < state->count; i++) {

		assert_true(
			expression_equals(&state->test_cases[i].expression, &state->test_cases[i].expression)
		);
	}
}

static void test_expression_clone(void **state_) {
	struct test_state *state = *state_;

	for (size_t i = 0; i < state->count; i++) {
		struct expression clone = expression_clone(&state->test_cases[i].expression);

		assert_expression_equal(&clone, &state->test_cases[i].expression);

		expression_drop(&clone);
	}
}

static void test_expression_from_string(void **state_) {
	struct test_state *state = *state_;

	for (size_t i = 0; i < state->count; i++) {
		struct expression expression = expression_from_string(state->test_cases[i].string);

		assert_expression_equal(&expression, &state->test_cases[i].expression);

		expression_drop(&expression);
	}
}

static void test_expression_to_string(void **state_) {
	struct test_state *state = *state_;

	for (size_t i = 0; i < state->count; i++) {
		char *string = expression_to_string(&state->test_cases[i].expression);

		assert_string_equal(string, state->test_cases[i].string);

		free(string);
	}
}

int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_expression_equals),
		cmocka_unit_test(test_expression_clone),
		cmocka_unit_test(test_expression_from_string),
		cmocka_unit_test(test_expression_to_string),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}
