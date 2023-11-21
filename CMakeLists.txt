cmake_minimum_required(VERSION 3.22)

project(arviss_cpp
    VERSION 0.1
    DESCRIPTION "Arviss cpp - A RISC-V Instruction Set Simulator"
    LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(arviss_cpp
    arviss.cpp
    basic_mem.h
    core_concepts.h
    core_mixins.h
    instruction.h
    rv32i_basic_cpu.h
    rv32i_concepts.h
    rv32i_cpu.h
    rv32i_disassembler.h
    rv32i_dispatcher.h
    rv32i_instruction_handler.h
    types.h)
