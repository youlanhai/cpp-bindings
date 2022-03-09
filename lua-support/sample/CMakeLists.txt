
set(TARGET_NAME sample)

file(GLOB SOURCE_FILES *.hpp *.cpp gen/*.cpp)

add_executable(${TARGET_NAME} ${SOURCE_FILES})

include_directories(${LUA_PATH} "${PROJECT_SOURCE_DIR}/lslua")
target_link_libraries(${TARGET_NAME} lslua lua)
