add_library(refcpp-utils SHARED StructuralContext.cpp)

add_executable(example_refcpp-utils-test test.cpp)
target_link_libraries(example_refcpp-utils-test refcpp-utils example_company)
