cmake_minimum_required(VERSION 3.24)

project(JSONC VERSION 1.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JSONC_LIBRARY_KIND "static" CACHE STRING "Build jsonc as static or shared library")
set_property(CACHE JSONC_LIBRARY_KIND PROPERTY STRINGS static shared)

option(JSONC_BUILD_C_API "Build the compiled C API library" ON)
option(JSONC_ENABLE_TEST "Build the test executable" OFF)

if(NOT JSONC_LIBRARY_KIND STREQUAL "static" AND NOT JSONC_LIBRARY_KIND STREQUAL "shared")
	message(FATAL_ERROR "JSONC_LIBRARY_KIND must be either 'static' or 'shared'.")
endif()

if(JSONC_LIBRARY_KIND STREQUAL "shared")
	set(jsonc_target_type SHARED)
else()
	set(jsonc_target_type STATIC)
endif()

add_library(jsonc INTERFACE)
add_library(JSONC::jsonc ALIAS jsonc)

set(JSONC_PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
if(NOT JSONC_BUILD_C_API)
	# Keep C++ header layout while stripping C API headers from consumers.
	set(JSONC_PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/jsonc-public-include")
	file(REMOVE_RECURSE "${JSONC_PUBLIC_INCLUDE_DIR}")
	file(MAKE_DIRECTORY "${JSONC_PUBLIC_INCLUDE_DIR}/sculk")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/include/sculk/jsonc" DESTINATION "${JSONC_PUBLIC_INCLUDE_DIR}/sculk")
endif()

target_include_directories(
	jsonc
	INTERFACE
	"${JSONC_PUBLIC_INCLUDE_DIR}"
)

target_compile_features(jsonc INTERFACE cxx_std_23)

file(GLOB_RECURSE JSONC_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

include(CheckIPOSupported)

function(jsonc_normalized_arch out_var)
	string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" jsonc_arch)
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		if(CMAKE_VS_PLATFORM_NAME)
			string(TOLOWER "${CMAKE_VS_PLATFORM_NAME}" jsonc_arch)
		elseif(CMAKE_GENERATOR_PLATFORM)
			string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" jsonc_arch)
		elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
			set(jsonc_arch "x64")
		endif()
	endif()

	if(jsonc_arch STREQUAL "x64" OR jsonc_arch STREQUAL "amd64")
		set(jsonc_arch "x86_64")
	endif()

	set(${out_var} "${jsonc_arch}" PARENT_SCOPE)
endfunction()

jsonc_normalized_arch(JSONC_ARCH)

set(JSONC_PLATFORM "${CMAKE_SYSTEM_NAME}")
string(TOLOWER "${JSONC_PLATFORM}" JSONC_PLATFORM)
set(JSONC_SHARED_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/jsonc-${JSONC_PLATFORM}-${JSONC_ARCH}")
set(JSONC_ARTIFACTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/artifacts")

if(JSONC_BUILD_C_API)
	add_library(jsonc_capi ${jsonc_target_type} ${JSONC_SOURCES})
	add_library(JSONC::capi ALIAS jsonc_capi)

	target_link_libraries(jsonc_capi PUBLIC JSONC::jsonc)
	target_include_directories(jsonc_capi PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
	target_compile_features(jsonc_capi PUBLIC cxx_std_23)

	if(NOT WIN32)
		set_target_properties(jsonc_capi PROPERTIES POSITION_INDEPENDENT_CODE ON)
	endif()

	if(JSONC_LIBRARY_KIND STREQUAL "shared")
		set_target_properties(jsonc_capi PROPERTIES PREFIX "lib")
		target_compile_definitions(jsonc_capi
			PRIVATE
				_JSONC_EXPORT
			INTERFACE
				JSONC_DLL
		)
	endif()

	if(MSVC)
		target_compile_definitions(jsonc_capi PRIVATE NOMINMAX UNICODE)
		target_compile_options(jsonc_capi PRIVATE
			/utf-8
			/W4
			/GR-
			/EHs-c-
			$<$<CONFIG:Release>:/O2>
			$<$<CONFIG:Release>:/Ob3>
		)
	else()
		target_compile_options(jsonc_capi PRIVATE
			-Wall
			-Wextra
			-Wconversion
			-pedantic
			-fno-exceptions
			-fPIC
			-stdlib=libc++
			-fno-rtti
			$<$<CONFIG:Release>:-O3>
		)
	endif()

	if(JSONC_LIBRARY_KIND STREQUAL "shared")
		check_ipo_supported(RESULT jsonc_ipo_supported OUTPUT jsonc_ipo_output)
		if(jsonc_ipo_supported)
			set_property(TARGET jsonc_capi PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()

		if(APPLE)
			set_target_properties(jsonc_capi PROPERTIES BUILD_WITH_INSTALL_NAME_DIR ON INSTALL_NAME_DIR "@rpath")
		elseif(UNIX)
			target_compile_options(jsonc_capi PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
			target_link_options(jsonc_capi PRIVATE
				-static-libstdc++
				-static-libgcc
				-Wl,--no-undefined
				-Wl,--exclude-libs,ALL
			)
			target_link_libraries(jsonc_capi PRIVATE pthread c++)
		endif()

		add_custom_command(TARGET jsonc_capi POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${JSONC_SHARED_OUTPUT_DIR}"
			COMMAND ${CMAKE_COMMAND} -E make_directory "${JSONC_ARTIFACTS_DIR}"
			COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:jsonc_capi>" "${JSONC_SHARED_OUTPUT_DIR}/"
			COMMAND ${CMAKE_COMMAND} -E tar cfv "${JSONC_ARTIFACTS_DIR}/jsonc-${JSONC_PLATFORM}-${JSONC_ARCH}.zip" --format=zip .
			WORKING_DIRECTORY "${JSONC_SHARED_OUTPUT_DIR}"
			VERBATIM
			COMMENT "Copying and packaging shared library artifacts"
		)
	endif()
endif()

if(JSONC_ENABLE_TEST)
	file(GLOB_RECURSE JSONC_TEST_SOURCES CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
	)

	add_executable(test ${JSONC_TEST_SOURCES})
	target_include_directories(test PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/test"
	)
	target_link_libraries(test PRIVATE JSONC::jsonc)
	target_compile_definitions(test PRIVATE JSONC_USE_EXPECTED)

	if(MSVC)
		target_compile_definitions(test PRIVATE NOMINMAX UNICODE)
		target_compile_options(test PRIVATE /utf-8 /W4)
	else()
		target_compile_options(test PRIVATE -Wall -Wextra -stdlib=libc++)
		target_link_libraries(test PRIVATE c++)
	endif()

	add_custom_command(TARGET test POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/bin"
		COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:test>" "${CMAKE_CURRENT_SOURCE_DIR}/bin/"
		VERBATIM
		COMMENT "Copying test executable to bin"
	)
endif()
