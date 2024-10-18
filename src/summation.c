#include <summation.h>

#include <assert.h>
#include <stdint.h>

double summation(long lower_bound, long upper_bound, const char *summand) {
	assert(summand != NULL);

	if (lower_bound > upper_bound) {
		return 0;
	}

	struct expression expression = expression_from_string(summand);
	struct environment environment = environment_new();

	expression_simplify(&expression, &environment);

	double sum = 0;
	for (long index = lower_bound; index <= upper_bound; ++index) {
		environment_set_variable(&environment, 'i', (double)index);

		sum += expression_evaluate(&expression, &environment);
	}

	expression_drop(&expression);

	return sum;
}
