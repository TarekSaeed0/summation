#include <environment.h>

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stddef.h>

struct environment environment_new(void) {
	struct environment environment;

	for (size_t i = 0; i < sizeof(environment.variables) / sizeof(environment.variables[0]); i++) {
		environment.variables[i] = NAN;
	}

	return environment;
}

double environment_get_variable(const struct environment *environment, char name) {
	assert(environment != NULL && isalpha(name));

	return environment->variables[name - 'a'];
}

void environment_set_variable(struct environment *environment, char name, double value) {
	assert(environment != NULL && isalpha(name));

	environment->variables[name - 'a'] = value;
}
