cmake_minimum_required(VERSION 3.15)
project(MoniteurSysteme C)

find_package(Threads REQUIRED)

add_executable(MoniteurSysteme
    src/main.c
    src/process.c
    src/cpu.c
    src/memoryInfo.c
)
