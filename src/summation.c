#include <summation.h>

#include <assert.h>

double summation(size_t lower_bound, size_t upper_bound, const char *summand) {
	assert(summand != NULL);

	if (lower_bound > upper_bound) {
		return 0;
	}

	struct expression expression = expression_from_string(summand);
	struct environment environment = environment_new();

	double sum = 0;
	for (size_t index = lower_bound; index <= upper_bound; ++index) {
		environment_set_variable(&environment, 'i', (double)index);

		sum += expression_evaluate(expression, &environment);
	}

	expression_drop(expression);

	return sum;
}
