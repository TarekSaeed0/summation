set(CMOCKA_TESTS test_environment test_expression test_summation)

foreach(_CMOCKA_TEST ${CMOCKA_TESTS})
	add_cmocka_test(
		${_CMOCKA_TEST}
		SOURCES
		../src/environment.c
		../src/expression.c
		../src/summation.c
		${_CMOCKA_TEST}.c
		COMPILE_OPTIONS
		${DEFAULT_C_COMPILE_FLAGS}
		LINK_LIBRARIES
		cmocka::cmocka
		m
		LINK_OPTIONS
		${DEFAULT_LINK_FLAGS}
	)
	target_include_directories(
		${_CMOCKA_TEST} PRIVATE ../include ${cmocka_BINARY_DIR}
	)

	add_cmocka_test_environment(${_CMOCKA_TEST})
endforeach()
