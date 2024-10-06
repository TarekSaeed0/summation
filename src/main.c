#include <expression.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMUM_INPUT_SIZE 1024

int main(void) {
	size_t lower_bound = 0;
	printf("Enter the lower bound: ");
	if (scanf("%zu", &lower_bound) != 1) {
		(void)fprintf(stderr, "error: failed to read the lower bound\n");
		return EXIT_FAILURE;
	}

	size_t upper_bound = 0;
	printf("Enter the upper bound: ");
	if (scanf("%zu", &upper_bound) != 1) {
		(void)fprintf(stderr, "error: failed to read the upper bound\n");
		return EXIT_FAILURE;
	}

	// flushing stdin
	int character = 0;
	while ((character = getchar()) != '\n' && character != EOF) {}

	printf("Enter the expression: ");
	char input[MAXIMUM_INPUT_SIZE];
	if (fgets(input, sizeof(input), stdin) == NULL) {
		(void)fprintf(stderr, "error: failed to read the expression\n");
		return EXIT_FAILURE;
	}
	input[strcspn(input, "\n")] = '\0';

	struct expression expression = expression_from_string(input);
	struct expression_environment environment = { .variables = { 0 } };

	double sum = 0;
	for (size_t index = lower_bound; index <= upper_bound; ++index) {
		environment.variables['i' - 'a'] = (double)index;
		sum += expression_evaluate(expression, &environment);
	}

	printf("The sum is %lg\n", sum);

	expression_drop(expression);
}
