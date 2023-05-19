set(${TARGET} Clone.Test)
set(OUTPUT_NAME Clone.Test)

set(SOURCE_FILES
    Tests.cpp
)

set(BENCHMARK_SOURCE_FILES
    Benchmark.cpp
)

add_executable(Clone.Test ${SOURCE_FILES})
#add_executable(Clone.Benchmark ${BENCHMARK_SOURCE_FILES})

set_target_properties(Clone.Test PROPERTIES FOLDER ${FOLDER})

target_link_libraries(Clone.Test PRIVATE Catch2::Catch2WithMain)
#target_link_libraries(Clone.Benchmark PRIVATE Clone.Core)

#include(CTest)
#include(Catch)

# add_test(NAME ${OUTPUT_NAME} COMMAND Clone.Test)
