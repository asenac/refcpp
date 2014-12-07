add_executable(example_person person.cpp)
target_link_libraries(example_person refcpp)

add_library(example_company SHARED company.cpp)

add_executable(example_graphviz graphviz.cpp)
target_link_libraries(example_graphviz refcpp example_company)
