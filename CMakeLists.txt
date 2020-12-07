cmake_minimum_required(VERSION 3.16)
project(SA_TSP)

set(CMAKE_CXX_STANDARD 14)

add_executable(SA_TSP main.cpp SA.h SA.cpp GA.h GA.cpp Greedy.cpp Greedy.h Nearest.h Nearest.cpp OPT-2.h OPT-2.cpp AntColony.cpp AntColony.h Tabu.cpp Tabu.h ChainPath.cpp ChainPath.h)