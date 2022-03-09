
set(TARGET_NAME lslua)

file(GLOB SOURCE_FILES *.hpp *.cpp)

include_directories(${LUA_PATH})
add_library(${TARGET_NAME} STATIC  ${SOURCE_FILES})
