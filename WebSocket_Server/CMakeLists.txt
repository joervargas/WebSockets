project(WebSocket_Server)

FILE(GLOB SRC_FILES ${PROJECT_NAME}/*.cpp)
# FILE(GLOB HDR_FILES ${PROJECT_NAME}/*.hpp)

set(WebSocket_DIR ../WebSocket)
include_directories(${WebSocket_DIR})

add_executable(${PROJECT_NAME} 
    ${SRC_FILES}
    # ${HDR_FILES}
)

target_link_libraries(${PROJECT_NAME} 
    WebSocket
)