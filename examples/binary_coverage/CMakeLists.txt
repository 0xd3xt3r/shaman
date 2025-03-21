cmake_minimum_required(VERSION 3.5.0)

project("binary_coverage")

include(FetchContent) 

FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG v2.4.2
)

FetchContent_MakeAvailable(cli11)

add_executable(${PROJECT_NAME})
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

set(CMAKE_PREFIX_PATH "../../builds/shaman_lib")

find_package(ShamanDBA CONFIG REQUIRED)
# find_package(spdlog CONFIG REQUIRED)

target_sources(${PROJECT_NAME}
    PRIVATE
    main.cpp
)


target_link_libraries(${PROJECT_NAME} PUBLIC ShamanDBA CLI11::CLI11 spdlog)

# -------------------------------- { Coverage Consumer Application} --------------------------------
set(COVERAGE_CONSUME_APP "binary_coverage_consumer")

project(${COVERAGE_CONSUME_APP})

add_executable(${COVERAGE_CONSUME_APP})

target_sources(${COVERAGE_CONSUME_APP}
    PRIVATE
    coverage_consumer.cpp
)

target_link_libraries(${COVERAGE_CONSUME_APP} PUBLIC ShamanDBA CLI11::CLI11 spdlog)