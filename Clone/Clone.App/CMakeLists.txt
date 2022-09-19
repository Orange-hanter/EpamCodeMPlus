project(Clone.App LANGUAGES CXX)
set(TARGET ${PROJECT_NAME})
set(OUTPUT_NAME ${PROJECT_NAME})

set(SOURCE_FILES

    Strurtup.hpp
    main.cpp
)

set(LIB_SOURCE_FILES
    Director.hpp
    Director.cpp
    iWorker.hpp
    Package.hpp
)

add_executable(${TARGET} ${SOURCE_FILES})

add_library(Copy.Core STATIC ${LIB_SOURCE_FILES})

set_target_properties(${TARGET} PROPERTIES FOLDER ${FOLDER})

target_link_libraries(${TARGET} argparse::argparse)
target_link_libraries(${TARGET} Copy.Core)

install(TARGETS ${TARGET}
    CONFIGURATIONS Debug
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/)
install(TARGETS ${TARGET}
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/)