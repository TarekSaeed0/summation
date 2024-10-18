#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include <environment.h>
#include <math.h>

#define EPSILON (0.000000001)

static void test_environment(void **state) {
	(void)state;

	struct environment environment = environment_new();

	environment_set_variable(&environment, 'x', 1.5);
	assert_float_equal(environment_get_variable(&environment, 'x'), 1.5, EPSILON);

	environment_set_variable(&environment, 'x', -5);
	assert_float_equal(environment_get_variable(&environment, 'x'), -5, EPSILON);

	environment_set_variable(&environment, 'A', 2);
	assert_float_equal(environment_get_variable(&environment, 'x'), -5, EPSILON);
	assert_float_equal(environment_get_variable(&environment, 'A'), 2, EPSILON);
}
int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_environment),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
