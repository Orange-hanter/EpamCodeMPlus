set(TARGET Clone.Core)
set(FOLDER ${PROJECT_NAME})

set(LIB_SOURCE_FILES
        WorkerFactory.cpp
        Factories/NetworkWorkerFactory.cpp
)

set(LIB_HEADER_FILES
        Factories/AbstractWorkerFactory.hpp
        Factories/NetworkWorkerFactory.hpp
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
        TransportLayer/TCPTransportLayer.hpp
        TransportLayer/iTransportLayer.hpp)

set(SOURCE_FILES ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES} )

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET} PUBLIC ./Common
                                            ./Workers
                                            ./Factories
                                            ./TransportLayer)

target_link_libraries(${TARGET} PUBLIC asio::asio)