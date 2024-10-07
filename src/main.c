#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <summation.h>

#define DEFAULT_BASE 10

/**
 * @brief Converts a string to a size_t
 *
 * Parses the string `string` into a size_t and stores it into `result`.
 *
 * @param[in] string The string to be parsed
 * @param[out] result Pointer to the size_t to store the result
 * @return EXIT_SUCCESS on success, and EXIT_FAILURE on error
 */
static inline int strtozu(const char *string, size_t *result) {
	assert(string != NULL && result != NULL);

	char *end = NULL;
	uintmax_t value = strtoumax(string, &end, DEFAULT_BASE);
	if (end == string) {
		(void)fprintf(stderr, "Error: Failed to parse a number from \"%s\"\n", string);
		return EXIT_FAILURE;
	}

	while (isspace(*end)) {
		++end;
	}
	if (*end != '\0') {
		(void)fprintf(stderr, "Error: Trailing characters after number \"%s\"\n", end);
		return EXIT_FAILURE;
	}

	if (value > SIZE_MAX) {
		(void)fprintf(stderr, "Error: Number is out of the representable range \"%s\"\n", string);
		return EXIT_FAILURE;
	}

	*result = value;

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		(void)fprintf(stderr, "Usage: %s LOWER_BOUND UPPER_BOUND SUMMAND\n", argv[0]);
		return EXIT_FAILURE;
	}

	size_t lower_bound = 0;
	if (strtozu(argv[1], &lower_bound) == EXIT_FAILURE) {
		(void)fprintf(stderr, "Error: Invalid lower bound \"%s\"\n", argv[1]);
		return EXIT_FAILURE;
	}

	size_t upper_bound = 0;
	if (strtozu(argv[2], &upper_bound) == EXIT_FAILURE) {
		(void)fprintf(stderr, "Error: Invalid upper bound \"%s\"\n", argv[2]);
		return EXIT_FAILURE;
	}

	printf("%lg\n", summation(lower_bound, upper_bound, argv[3]));
}
