project(Clone.Core LANGUAGES CXX)
set(TARGET ${PROJECT_NAME})
set(FOLDER ${PROJECT_NAME})

set(HEADER_FILES_APP
)

set(LIB_SOURCE_FILES
    ./WorkerFactory.cpp
)

set(LIB_HEADER_FILES
./AbstractWorkerFactory.hpp
./WorkerFactory.hpp
./Workers/iWorker.hpp
./Workers/Workers.h
./Workers/ByteStreamWorker.hpp
./Workers/IPCWorker.hpp
./Common/SafeDeque.hpp
./Common/Frame.hpp
./Common/Utils.hpp
./Common/Configuration.hpp
)

set(SOURCE_FILES ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES})

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET} PUBLIC    ${CMAKE_CURRENT_SOURCE_DIR}/Common 
                                                ${CMAKE_CURRENT_SOURCE_DIR}/Workers 
                                                 ${CMAKE_CURRENT_SOURCE_DIR})
                                            