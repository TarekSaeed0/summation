#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

#include <summation.h>

#define EPSILON (0.000000001)

static const struct test_case {
	double lower_bound;
	double upper_bound;
	const char *summand;
	double summation;
} test_cases[] = {
	{
		.lower_bound = 1,
		.upper_bound = 10,
		.summand = "1",
		.summation = 10,
	},
	{
		.lower_bound = 1,
		.upper_bound = 20,
		.summand = "2.5",
		.summation = 50,
	},
	{
		.lower_bound = -15,
		.upper_bound = 2,
		.summand = "i",
		.summation = -117,
	},
	{
		.lower_bound = 210,
		.upper_bound = 74,
		.summand = "i^2 + 2",
		.summation = 0,
	},
	{
		.lower_bound = 10,
		.upper_bound = 10,
		.summand = "i * (i + 1) / 2",
		.summation = 55,
	},
	{ .lower_bound = 0, .upper_bound = 5, .summand = "i^3 / (i + 1)", .summation = 43.55 },
	{
		.lower_bound = -3,
		.upper_bound = 7,
		.summand = "i^i",
		.summation = 873612.212963,
	}
};

static void test_summation(void **state) {
	(void)state;

	for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
		assert_float_equal(
			summation(test_cases[i].lower_bound, test_cases[i].upper_bound, test_cases[i].summand),
			test_cases[i].summation,
			EPSILON
		);
	}
}
int main(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_summation),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
