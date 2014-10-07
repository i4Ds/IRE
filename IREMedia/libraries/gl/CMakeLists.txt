project(gl)

file(GLOB SOURCE_FILES *.h *.cpp)

add_library(gl STATIC ${SOURCE_FILES})
