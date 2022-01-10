cmake_minimum_required(VERSION 3.20)
project(Algorithmic-Programming)

set(CMAKE_CXX_STANDARD 11)

add_executable(Algorithmic-Programming anomaly_detection_util.h anomaly_detection_util.cpp AnomalyDetector.h CLI.h CLI.cpp commands.h HybridAnomalyDetector.h HybridAnomalyDetector.cpp SimpleAnomalyDetector.h SimpleAnomalyDetector.cpp timeseries.h timeseries.cpp minCircle.h minCircle.cpp MainTrain.cpp)
