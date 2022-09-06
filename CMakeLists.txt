cmake_minimum_required(VERSION 3.10)

project(JobScheduler
    DESCRIPTION "A simple Job Scheduler"
    LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(src)

set(SRC_DIR src)

add_executable(JobScheduler
    ${SRC_DIR}/main.cc
)

target_link_libraries(JobScheduler PRIVATE Threads::Threads)
