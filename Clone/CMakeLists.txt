project(Clone LANGUAGES CXX)

set(TARGET Clone)
set(FOLDER "Clone")

add_subdirectory(Clone.App)
add_subdirectory(Core)
add_subdirectory(Test)

add_library(${TARGET} INTERFACE)
