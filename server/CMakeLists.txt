project(r-type_server)

message("Declaring Boost...")
FetchContent_Declare(Boost GIT_REPOSITORY https://github.com/boostorg/boost.git GIT_TAG boost-1.80.0)

message("Fetching Boost...")
set(Boost_USE_STATIC_LIBS ON)
FetchContent_MakeAvailable(Boost)

include_directories(
    ${boost_fusion_SOURCE_DIR}/include
    ${boost_unordered_SOURCE_DIR}/include
    ${boost_regex_SOURCE_DIR}/include
    ${boost_tokenizer_SOURCE_DIR}/include
    ${boost_function_types_SOURCE_DIR}/include
    ${boost_conversion_SOURCE_DIR}/include
    ${boost_range_SOURCE_DIR}/include
    ${boost_optional_SOURCE_DIR}/include
    ${boost_numeric_conversion_SOURCE_DIR}/include
    ${boost_type_index_SOURCE_DIR}/include
    ${boost_iterator_SOURCE_DIR}/include
    ${boost_tuple_SOURCE_DIR}/include
    ${boost_utility_SOURCE_DIR}/include
    ${boost_lexical_cast_SOURCE_DIR}/include
    ${boost_array_SOURCE_DIR}/include
    ${boost_smart_ptr_SOURCE_DIR}/include
    ${boost_function_SOURCE_DIR}/include
    ${boost_detail_SOURCE_DIR}/include
    ${boost_ratio_SOURCE_DIR}/include
    ${boost_date_time_SOURCE_DIR}/include
    ${boost_io_SOURCE_DIR}/include
    ${boost_rational_SOURCE_DIR}/include
    ${boost_algorithm_SOURCE_DIR}/include
    ${boost_typeof_SOURCE_DIR}/include
    ${boost_exception_SOURCE_DIR}/include
    ${boost_intrusive_SOURCE_DIR}/include
    ${boost_mpl_SOURCE_DIR}/include
    ${boost_container_hash_SOURCE_DIR}/include
    ${boost_predef_SOURCE_DIR}/include
    ${boost_preprocessor_SOURCE_DIR}/include
    ${boost_move_SOURCE_DIR}/include
    ${boost_container_SOURCE_DIR}/include
    ${boost_winapi_SOURCE_DIR}/include
    ${boost_concept_check_SOURCE_DIR}/include
    ${boost_align_SOURCE_DIR}/include
    ${boost_integer_SOURCE_DIR}/include
    ${boost_mp11_SOURCE_DIR}/include
    ${boost_core_SOURCE_DIR}/include
    ${boost_variant2_SOURCE_DIR}/include
    ${boost_chrono_SOURCE_DIR}/include
    ${boost_type_traits_SOURCE_DIR}/include
    ${boost_static_assert_SOURCE_DIR}/include
    ${boost_throw_exception_SOURCE_DIR}/include
    ${boost_bind_SOURCE_DIR}/include
    ${boost_config_SOURCE_DIR}/include
    ${boost_atomic_SOURCE_DIR}/include
    ${boost_assert_SOURCE_DIR}/include
    ${boost_system_SOURCE_DIR}/include
    ${boost_thread_SOURCE_DIR}/include
    ${boost_asio_SOURCE_DIR}/include
)


file(GLOB_RECURSE SRC *.cpp *.hpp)

set(CMAKE_BUILD_TYPE Debug)
add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} boost_system boost_thread)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
    target_link_options(${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()