project(rtype)
cmake_minimum_required(VERSION 3.15)

add_compile_definitions(SILVA_STATE_DRAW)
add_compile_definitions(SILVA_STATE_HANDLE_EVENT)

add_compile_definitions(_WIN32_WINNT=0x0601)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)

message("Declaring SFML...")
FetchContent_Declare(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.6.x)
message("Fetching SFML...")
FetchContent_MakeAvailable(SFML)

message("Declaring hl...")
FetchContent_Declare(hl GIT_REPOSITORY https://github.com/HelifeWasTaken/hl)
message("Fetchting hl...")
FetchContent_MakeAvailable(hl)

message("Declaring Boost...")
FetchContent_Declare(Boost GIT_REPOSITORY https://github.com/boostorg/boost.git GIT_TAG boost-1.80.0)
message("Fetching Boost...")
set(Boost_USE_STATIC_LIBS ON)
FetchContent_MakeAvailable(Boost)

message("Declaring spdlog")
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.10.0)
message("Fetching spdlog")
FetchContent_MakeAvailable(spdlog)

include_directories(
	${SFML_SOURCE_DIR}/include
	${hl_SOURCE_DIR}/include
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
    ${boost_variant_SOURCE_DIR}/../predef/include # more crimes
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
    ${spdlog_SOURCE_DIR}/include
    ${boost_lockfree_SOURCE_DIR}/include
    ${boost_parameter_SOURCE_DIR}/include
    ${boost_variant_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/include
)

message(    ${boost_fusion_SOURCE_DIR}/include
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
${boost_lockfree_SOURCE_DIR}/include
${boost_parameter_SOURCE_DIR}/include
${boost_variant_SOURCE_DIR}/include)

add_subdirectory(server)

add_compile_definitions(RTYPE_USING_POC_MAIN)
add_subdirectory(POC)

add_compile_definitions(RTYPE_BUILDING_CLIENT)
add_subdirectory(client)
