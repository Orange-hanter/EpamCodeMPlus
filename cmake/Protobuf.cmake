set(TARGET proto)
set(PROTO_FILES ${CMAKE_SOURCE_DIR}/messages.proto)

set(PROTO_GEN_PATH ${CMAKE_SOURCE_DIR}/Src/Core/Messages)
file(MAKE_DIRECTORY ${PROTO_GEN_PATH})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_GEN_PATH} ${PROTO_FILES})

#add_library(${TARGET} ${PROTO_SRCS} ${PROTO_HDRS})

#target_include_directories(${TARGET} PUBLIC ${PROTO_GEN_PATH})
