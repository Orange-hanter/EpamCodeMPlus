set(TARGET proto-objects)

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(PROTO_FILES ./messages.proto)


add_library(${TARGET} OBJECT ${PROTO_FILES})
target_link_libraries(${TARGET} PUBLIC protobuf::libprotobuf)
#target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(proto-objects PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

protobuf_generate(
        TARGET ${TARGET}
        LANGUAGE cpp
        OUT_VAR PROTO_GEN_FILES
        IMPORT_DIRS "./"
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)
