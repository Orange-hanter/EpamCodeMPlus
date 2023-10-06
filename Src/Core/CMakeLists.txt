set(TARGET Clone.Core)
set(FOLDER ${PROJECT_NAME})

# Protobuf gen files
add_subdirectory(Messages)

set(LIB_SOURCE_FILES
        WorkerFactory.cpp
        Factories/NetworkWorkerFactory.cpp
        TransportLayer/Downloader.cpp
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
        TransportLayer/TCPAcceptor.hpp
        TransportLayer/Downloader.hpp
        TransportLayer/Connection.hpp
        TransportLayer/DlManager.hpp
        Factories/IPCWorkerFactory.hpp
        Factories/IPCWorkerFactory.cpp
        Factories/BytestreamWorkerFactory.cpp
        Factories/BytestreamWorkerFactory.hpp
        TransportLayer/Uploader.cpp
        TransportLayer/Uploader.hpp
        TransportLayer/Criptography.cpp
        TransportLayer/Criptography.hpp
)

set(SOURCE_FILES ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES} )

add_library(${TARGET} STATIC ${SOURCE_FILES})

target_include_directories(${TARGET} PUBLIC ./Common
                                            ./Workers
                                            ./Factories
                                            ./TransportLayer
                                            ./Messages
                                            ./
)

target_link_libraries(${TARGET} PUBLIC asio::asio proto-objects Boost::log_setup Boost::log cryptopp::cryptopp)
