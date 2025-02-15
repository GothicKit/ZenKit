add_executable(load_vdf load_vdf.cc)
target_link_libraries(load_vdf PRIVATE zenkit)

add_executable(load_zen load_zen.cc)
target_link_libraries(load_zen PRIVATE zenkit)

add_executable(run_interpreter run_interpreter.cc)
target_link_libraries(run_interpreter PRIVATE zenkit)

add_executable(zen2zen zen2zen.cc)
target_link_libraries(zen2zen PRIVATE zenkit)

set_target_properties(load_vdf load_zen run_interpreter zen2zen
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/examples"
		)
