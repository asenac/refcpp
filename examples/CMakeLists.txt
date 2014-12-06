add_executable(example_person person.cpp)
target_link_libraries(example_person refcpp)

add_library(example_company SHARED company.cpp)

add_subdirectory(utils)
