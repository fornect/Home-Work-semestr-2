cmake_minimum_required(VERSION 3.16)

project(csv_pretty_printer VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(csvpp CSVPP/CSVPP.c)
target_include_directories(csvpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CSVPP)
target_compile_options(csvpp PRIVATE -Wall -Wextra -Wpedantic)

add_executable(csv-pretty-printer CSVPP/main.c)
target_link_libraries(csv-pretty-printer PRIVATE csvpp)
target_compile_options(csv-pretty-printer PRIVATE -Wall -Wextra -Wpedantic)

include(CTest)
if(BUILD_TESTING)
    add_executable(csvpp-tests tests/test_csvpp.c)
    target_link_libraries(csvpp-tests PRIVATE csvpp)
    target_compile_options(csvpp-tests PRIVATE -Wall -Wextra -Wpedantic)
    add_test(NAME csvpp-tests COMMAND csvpp-tests)
endif()
