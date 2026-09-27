cmake_minimum_required(VERSION 3.25)

project(empire C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(CTest)

add_library(project_warnings INTERFACE)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wpedantic)
endif()

add_library(empire_distribution src/empire.c)
target_include_directories(empire_distribution PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(empire_distribution PRIVATE project_warnings)

add_executable(empire src/main.c)
target_link_libraries(empire PRIVATE empire_distribution project_warnings)

if(BUILD_TESTING)
    add_executable(empire_tests tests/test_empire.c)
    target_link_libraries(empire_tests PRIVATE empire_distribution project_warnings)
    add_test(NAME empire_tests COMMAND empire_tests)
endif()
