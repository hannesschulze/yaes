add_executable(nes_tool_generate_tiles)

target_include_directories(nes_tool_generate_tiles PRIVATE .)
target_include_directories(nes_tool_generate_tiles SYSTEM PRIVATE third-party)
target_link_libraries(nes_tool_generate_tiles PRIVATE nes::options)

target_sources(nes_tool_generate_tiles PRIVATE main.cc)

function(nes_generate_tiles TARGET_NAME NAME)
	cmake_parse_arguments(ARGS "" "TYPE" "FILES" ${ARGN})
	if(NOT DEFINED ARGS_TYPE)
		message(FATAL_ERROR "nes_generate_tiles: Missing TYPE")
	endif()
	if(NOT DEFINED ARGS_FILES)
		message(FATAL_ERROR "nes_generate_tiles: Missing FILES")
	endif()

	set(CODEGEN_TARGET "nes_tiles_${NAME}")
	set(CODEGEN_BASE "${CMAKE_CURRENT_BINARY_DIR}/generated")
	set(CODEGEN_DIR "${CODEGEN_BASE}/nes/app/graphics/tiles")
	set(CODEGEN_SOURCE "${CODEGEN_DIR}/${NAME}.cc")
	set(CODEGEN_HEADER "${CODEGEN_DIR}/${NAME}.hh")
	file(MAKE_DIRECTORY "${CODEGEN_DIR}")

	add_custom_command(
		OUTPUT "${CODEGEN_SOURCE}" "${CODEGEN_HEADER}"
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND nes_tool_generate_tiles
		ARGS
		"${ARGS_TYPE}"
		"${CODEGEN_HEADER}"
		"${CODEGEN_SOURCE}"
		${ARGS_FILES}
		VERBATIM
		DEPENDS ${ARGS_FILES})

	add_custom_target(${CODEGEN_TARGET} DEPENDS "${CODEGEN_SOURCE}" "${CODEGEN_HEADER}")
	target_include_directories(${TARGET_NAME} PRIVATE "${CODEGEN_BASE}")
	target_sources(${TARGET_NAME} PRIVATE "${CODEGEN_SOURCE}" "${CODEGEN_HEADER}")
	set_source_files_properties("${CODEGEN_SOURCE}" PROPERTIES GENERATED TRUE)
	set_source_files_properties("${CODEGEN_HEADER}" PROPERTIES GENERATED TRUE)
	add_dependencies(${TARGET_NAME} ${CODEGEN_TARGET})
endfunction()
