set(TARGET Clone.App)
set(OUTPUT_NAME ${TARGET})

set(SOURCE_FILES
    ./main.cpp
    ./Startup.hpp
)

add_executable(${TARGET} ${SOURCE_FILES})

target_link_libraries(${TARGET} PUBLIC  argparse::argparse
                                        Clone.Core)

install(TARGETS ${TARGET}
    CONFIGURATIONS Debug
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/)
install(TARGETS ${TARGET}
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin/)