cmake_minimum_required(VERSION 3.16)
project(Algorithm_TSP)

set(CMAKE_CXX_STANDARD 14)

add_executable(Algorithm src/main.cpp src/SA/SA.h src/SA/SA.cpp src/GA/GA.h src/GA/GA.cpp src/Greedy/Greedy.cpp src/Greedy/Greedy.h src/Nearest/Nearest.h src/Nearest/Nearest.cpp src/OPT-2/OPT-2.h src/OPT-2/OPT-2.cpp src/AntColony/AntColony.cpp src/AntColony/AntColony.h src/Tabu/Tabu.cpp src/Tabu/Tabu.h src/ChainPath/ChainPath.cpp src/ChainPath/ChainPath.h)