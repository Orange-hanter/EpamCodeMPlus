set(TARGET Test)
set(OUTPUT_NAME Clone.Test)
set(FOLDER ${TARGET})

set(SOURCE_FILES
    Tests.cpp
)

set(BENCHMARK_SOURCE_FILES
    Benchmark.cpp
)

add_executable(${TARGET} ${SOURCE_FILES})
#add_executable(Clone.Benchmark ${BENCHMARK_SOURCE_FILES})

#set_target_properties(${TARGET} PROPERTIES FOLDER ${FOLDER})

target_link_libraries(${TARGET} PUBLIC Catch2::Catch2WithMain)
#target_link_libraries(Clone.Benchmark PRIVATE Clone.Core)

#include(CTest)
#include(Catch)

# add_test(NAME ${OUTPUT_NAME} COMMAND Clone.Test)
