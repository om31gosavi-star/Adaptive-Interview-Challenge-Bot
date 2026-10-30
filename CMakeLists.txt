cmake_minimum_required(VERSION 3.15)
project(MockInterviewBot CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(MockInterviewBot
    src/main.cpp
    include/common.hpp
    include/ansi_terminal.hpp
    include/interviewer_persona.hpp
    include/question_bank.hpp
    include/adaptive_engine.hpp
    include/report_generator.hpp
    include/interview_engine.hpp
)

if (MSVC)
    target_compile_definitions(MockInterviewBot PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)
    target_compile_options(MockInterviewBot PRIVATE /utf-8 /W4)
endif()
