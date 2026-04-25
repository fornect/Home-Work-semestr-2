set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(avl_tree tree.c)

add_executable(aerosoft_app main.c)

target_link_libraries(aerosoft_app PRIVATE avl_tree)
