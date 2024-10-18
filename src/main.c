#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <summation.h>

#define DEFAULT_BASE 10

/**
 * @brief Converts a string to a long int
 *
 * Parses the string `string` into a long int and stores it into `result`.
 *
 * @param[in] string The string to be parsed
 * @param[out] result Pointer to the long int to store the result
 * @return EXIT_SUCCESS on success, and EXIT_FAILURE on error
 */
static inline int string_to_long(const char *string, long *result) {
	assert(string != NULL && result != NULL);

	char *end = NULL;
	long value = strtol(string, &end, DEFAULT_BASE);
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

	if (errno == ERANGE) {
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

	long lower_bound = 0;
	if (string_to_long(argv[1], &lower_bound) == EXIT_FAILURE) {
		(void)fprintf(stderr, "Error: Invalid lower bound \"%s\"\n", argv[1]);
		return EXIT_FAILURE;
	}

	long upper_bound = 0;
	if (string_to_long(argv[2], &upper_bound) == EXIT_FAILURE) {
		(void)fprintf(stderr, "Error: Invalid upper bound \"%s\"\n", argv[2]);
		return EXIT_FAILURE;
	}

	printf("%lg\n", summation(lower_bound, upper_bound, argv[3]));
}
