set(TARGET Clone.Core)
set(FOLDER ${PROJECT_NAME})

set(LIB_SOURCE_FILES
        WorkerFactory.cpp
)

set(LIB_HEADER_FILES
        AbstractWorkerFactory.hpp
        WorkerFactory.hpp
        Workers/iWorker.hpp
        Workers/Workers.h
        Workers/ByteStreamWorker.hpp
        Workers/NetworkWorker.hpp
        Workers/IPCWorker.hpp
        Common/SafeDeque.hpp
        Common/Frame.hpp
        Common/IPCFrame.hpp
        Common/Utils.hpp
        Common/Configuration.hpp
)

set(SOURCE_FILES ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES} )

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET} PUBLIC ./Common
                                            ./Workers)

target_link_libraries(${TARGET} PUBLIC asio::asio)