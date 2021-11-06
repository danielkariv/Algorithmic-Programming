cmake_minimum_required(VERSION 3.20)
project(Algorithmic-Programming)

set(CMAKE_CXX_STANDARD 11)

add_executable(Algorithmic-Programming MainTrain.cpp anomaly_detection_util.cpp SimpleAnomalyDetector.cpp timeseries.cpp)
