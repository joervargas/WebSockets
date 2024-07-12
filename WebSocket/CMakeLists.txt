project(WebSocket CXX)

message("Including FetchContent ...")
    include(FetchContent)
    message("Fetching SpdLog")
        FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG        v1.13.0
        )
    message("SpdLog fetched")

    message("Making SpdLog available")
        FetchContent_MakeAvailable(spdlog)
    message("SpdLog available")
message("FetchContent completed")

# FILE(GLOB SRC_FILES ${PROJECT_NAME}/*.cpp)
FILE(GLOB HDR_FILES ${PROJECT_NAME}/*.hpp)

add_library(${PROJECT_NAME} 
    ${SRC_FILES} 
    ${HDR_FILES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)