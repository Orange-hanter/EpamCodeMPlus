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
        TransportLayer/iTransportLayer.hpp
        TransportLayer/Downloader.hpp 
        TransportLayer/Connection.hpp
)

set(PROTOBUF_SRC 
                Messages/messages.pb.cc
                Messages/messages.pb.h
)

set(SOURCE_FILES ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES} ${PROTOBUF_SRC} TransportLayer/DlManager.hpp TransportLayer/Downloader.cpp)

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET} PUBLIC ./Common
                                            ./Workers
                                            ./Factories
                                            ./TransportLayer
                                            ./Messages
)

target_link_libraries(${TARGET} PUBLIC asio::asio)