#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#define VARIABLES_COUNT (('z' - 'a' + 1) + ('Z' - 'A' + 1))

/**
 * @brief an evaluation environment.
 *
 * This data structure represents the environment in which an expression is evaluated.
 */
struct environment {
	double variables[VARIABLES_COUNT]; ///< The values of the variables
};

/**
 * @brief Creates a new environment.
 *
 * Initalizes a new environment with all its variables set to NAN.
 *
 * @return The newly created environment.
 *
 * @memberof environment
 */
struct environment environment_new(void);

/**
 * @brief Retrieves the value of a variable.
 *
 * Returns the value stored from the variable with the given name in the given environment.
 *
 * @param[in] environment Pointer to the environment.
 * @param[in] name The variable's name, must be an alphabet letter.
 * @return The value of the variable.
 *
 * @memberof environment
 */
double environment_get_variable(const struct environment *environment, char name);

/**
 * @brief Assigns a value to a variable.
 *
 * Stores the given value to the variable with the given name in the given environment.
 *
 * @param[in,out] environment Pointer to the environment.
 * @param[in] name The variable's name, must be an alphabet letter.
 * @param[in] value The value to be stored.
 *
 * @memberof environment
 */
void environment_set_variable(struct environment *environment, char name, double value);

#endif
