add_executable(example_person person.cpp)
add_executable(example_mapping mapping.cpp)

add_library(example_company SHARED company.cpp)

add_subdirectory(utils)
