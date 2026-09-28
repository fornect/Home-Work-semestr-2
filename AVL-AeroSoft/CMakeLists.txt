add_library(avl_tree tree.c)
target_include_directories(avl_tree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(avl_tree PRIVATE project_warnings)

add_executable(aerosoft main.c)
target_link_libraries(aerosoft PRIVATE avl_tree project_warnings)

if(BUILD_TESTING)
    add_executable(avl_tree_tests tests/test_tree.c)
    target_link_libraries(avl_tree_tests PRIVATE avl_tree project_warnings)
    add_test(
        NAME avl_tree_tests
        COMMAND avl_tree_tests ${CMAKE_CURRENT_BINARY_DIR}/test-airports.txt
    )
endif()
