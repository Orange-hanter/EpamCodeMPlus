set(TARGET Clone)
set(FOLDER Clone)

add_subdirectory(Clone.App)
add_subdirectory(Core)

add_library(${TARGET} INTERFACE)
