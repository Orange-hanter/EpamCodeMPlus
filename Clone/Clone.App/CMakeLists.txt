project(Clone.App LANGUAGES CXX)
set(TARGET ${PROJECT_NAME})
set(OUTPUT_NAME ${PROJECT_NAME})

set(HEADER_FILES_APP
    Strurtup.hpp
)

set(SOURCE_FILES
    ./main.cpp
    ./Startup.hpp
)

set(LIB_SOURCE_FILES
    WorkerFactory.cpp
)

set(LIB_HEADER_FILES
    ./WorkerFactory.hpp
    ./Frame.hpp
    ./Workers/iWorker.hpp
    ./Workers/ByteStreamWorker.hpp
)

add_executable(${TARGET} ${SOURCE_FILES})

add_library(Copy.Core STATIC ${LIB_SOURCE_FILES} ${LIB_HEADER_FILES})
target_include_directories(Copy.Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

set_target_properties(${TARGET} PROPERTIES FOLDER ${FOLDER})

target_link_libraries(${TARGET} argparse::argparse)
target_link_libraries(${TARGET} Copy.Core)

install(TARGETS ${TARGET}
    CONFIGURATIONS Debug
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/)
install(TARGETS ${TARGET}
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/)