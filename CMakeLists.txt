cmake_minimum_required(VERSION 3.2)
project(ftrl_train)
FIND_PACKAGE ( Threads REQUIRED )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")

add_executable(ftrl_train ftrl_train.cpp src/DB/mapDB.h src/DB/vectorDB.h src/DB/virtualDB.h src/Trainer/FTRL.cpp
        src/Trainer/FTRL.h src/Unit/entityUnit.h src/Unit/modelUnit.h src/Utils/utils.cpp src/Utils/utils.h)
TARGET_LINK_LIBRARIES ( ftrl_train ${CMAKE_THREAD_LIBS_INIT} )

#[[add_executable(ftrl_predict ftrl_predict.cpp src/DB/mapDB.h src/DB/vectorDB.h src/DB/virtualDB.h src/Trainer/FTRL.cpp
        src/Trainer/FTRL.h src/Unit/entityUnit.h src/Unit/modelUnit.h src/Utils/utils.cpp src/Utils/utils.h)
TARGET_LINK_LIBRARIES ( ftrl_predict ${CMAKE_THREAD_LIBS_INIT} )]]
