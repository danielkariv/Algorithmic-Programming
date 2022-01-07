cmake_minimum_required(VERSION 3.20)
project(Algorithmic-Programming)

set(CMAKE_CXX_STANDARD 11)

add_executable(Algorithmic-Programming anomaly_detection_util.h anomaly_detection_util.cpp AnomalyDetector.h SimpleAnomalyDetector.h SimpleAnomalyDetector.cpp timeseries.h timeseries.cpp minCircle.h minCircle.cpp MainTrain_ex3.cpp)
