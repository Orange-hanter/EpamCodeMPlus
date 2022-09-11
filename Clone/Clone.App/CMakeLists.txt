project(Clone.App LANGUAGES CXX)
set(TARGET ${PROJECT_NAME})
set(OUTPUT_NAME ${PROJECT_NAME})

set(SOURCE_FILES
    Direcror.hpp
    Direcror.cpp
    iWorker.hpp
    Package.hpp
    Strurtup.hpp
    main.cpp
)

add_executable(${TARGET} ${SOURCE_FILES})

set_target_properties(${TARGET} PROPERTIES FOLDER ${FOLDER})

target_link_libraries(${TARGET} argparse::argparse)

install(TARGETS ${TARGET}
    CONFIGURATIONS Debug
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/)
install(TARGETS ${TARGET}
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/)