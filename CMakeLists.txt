cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(BUILD_COVERAGE "Build code coverage" OFF)

set(
  HUNTER_CACHE_SERVERS
  "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
  CACHE STRING "Default cache server"
)

include("cmake/HunterGate.cmake")

HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.314.tar.gz"
  SHA1 "95c47c92f68edb091b5d6d18924baabe02a6962a"
)

project(template VERSION 0.1.0)
string(APPEND CMAKE_CXX_FLAGS " -pedantic -Werror -Wall -Wextra")
string(APPEND CMAKE_CXX_FLAGS " -Wno-unused-command-line-argument")
string(APPEND CMAKE_CXX_FLAGS " -Wshadow -Wnon-virtual-dtor")

find_package(Threads)
hunter_add_package(asio)
find_package(asio CONFIG REQUIRED version 1.18.1)

hunter_add_package(Beast)
find_package(Beast CONFIG REQUIRED)

hunter_add_package(OpenSSL)
find_package(OpenSSL REQUIRED)

hunter_add_package(gumbo)
find_package(gumbo CONFIG REQUIRED)

hunter_add_package(Boost COMPONENTS
        program_options
        system
        filesystem
        regex)
find_package(Boost CONFIG REQUIRED
        program_options
        system
        filesystem
        regex)
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/crawler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/downloader.cpp
)

add_executable(demo
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp
)
add_executable(demo1
        ${CMAKE_CURRENT_SOURCE_DIR}/demo1/load.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>"
)
target_link_libraries(demo ${PROJECT_NAME})
target_link_libraries(demo1 ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
        Boost::program_options
        Beast::Beast
        Boost::system
        Boost::filesystem
        Boost::regex
        asio::asio_static
        OpenSSL::SSL
        OpenSSL::Crypto
        pthread
        gumbo::gumbo
)

if(BUILD_TESTS)
  add_executable(tests
          ${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cpp
  )
  target_link_libraries(tests ${PROJECT_NAME} GTest::gtest_main)
  enable_testing()
  add_test(NAME unit_tests COMMAND tests)
endif()

if(BUILD_COVERAGE)
  set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/coverage/cmake")
  find_package(codecov)
  string(APPEND CMAKE_CXX_FLAGS " --coverage")
  add_coverage(${PROJECT_NAME})
  add_coverage(tests)
  coverage_evaluate()
endif()

if(BUILD_DOCS)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/_docs)
  set(DOXYGEN_EXAMPLE_PATH examples)
  set(DOXYGEN_GENERATE_HTML NO)
  set(DOXYGEN_GENERATE_MAN YES)
  find_package(Doxygen)
  doxygen_add_docs(docs include)
  install(DIRECTORY ${DOXYGEN_OUTPUT_DIRECTORY}/man DESTINATION share)
endif()

include(CPackConfig.cmake)

install(TARGETS ${PROJECT_NAME} demo
  EXPORT "${PROJECT_NAME}-targets"
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

install(EXPORT "${PROJECT_NAME}-targets"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "lib/cmake"
)
