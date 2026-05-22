add_executable(test_type_traits test_type_traits.cpp)
target_link_libraries(test_type_traits PRIVATE mystl GTest::gtest_main)
add_test(NAME TypeTraits COMMAND test_type_traits)

add_executable(test_allocator test_allocator.cpp)
target_link_libraries(test_allocator PRIVATE mystl GTest::gtest_main)
add_test(NAME Allocator COMMAND test_allocator)

add_executable(test_iterator test_iterator.cpp)
target_link_libraries(test_iterator PRIVATE mystl GTest::gtest_main)
add_test(NAME Iterator COMMAND test_iterator)

add_executable(test_vector test_vector.cpp)
target_link_libraries(test_vector PRIVATE mystl GTest::gtest_main)
add_test(NAME Vector COMMAND test_vector)

add_executable(test_string test_string.cpp)
target_link_libraries(test_string PRIVATE mystl GTest::gtest_main)
add_test(NAME String COMMAND test_string)
