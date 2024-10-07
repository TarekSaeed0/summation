#ifndef SUMMATION_H
#define SUMMATION_H

#include <expression.h>

/**
 * @brief Evaluates a summation
 *
 * Returns the total of the summation of expression `summand` from `lower_bound` to `upper_bound`
 * inclusive. The index of summation is named i.
 *
 * @param[in] lower_bound The lower bonud of the summation
 * @param[in] upper_bound The upper bound of the summation
 * @param[in] summand The summand of the summation
 * @return The total of the summation
 */
double summation(size_t lower_bound, size_t upper_bound, const char *summand);

#endif
