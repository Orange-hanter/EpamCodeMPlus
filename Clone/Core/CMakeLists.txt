project(Clone.Core LANGUAGES CXX)
set(TARGET ${PROJECT_NAME})
set(FOLDER ${PROJECT_NAME})

set(HEADER_FILES_APP
)

set(LIB_SOURCE_FILES
    ./WorkerFactory.cpp
)

set(LIB_HEADER_FILES
    ./WorkerFactory.hpp
    ./iWorker.hpp
    ./ByteStreamWorker.hpp
    ./IPCWorker.hpp
    ./include/SafeDeque.hpp
    ./include/Frame.hpp
    ./include/Utils.hpp
)

set(SOURCE_FILES ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES})

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
                                            ${CMAKE_CURRENT_SOURCE_DIR})
                                            