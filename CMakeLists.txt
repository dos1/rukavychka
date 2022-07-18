cmake_minimum_required(VERSION 3.6)

project(rukavychka C)

SET(LIBSUPERDERPY_GAMENAME "rukavychka" CACHE INTERNAL "")
SET(LIBSUPERDERPY_GAMENAME_PRETTY "Rukavychka" CACHE INTERNAL "")

SET(LIBSUPERDERPY_APPID "com.holypangolin.Rukavychka")
SET(LIBSUPERDERPY_ORIENTATION "LANDSCAPE")
SET(LIBSUPERDERPY_VERSION "1.0")
SET(LIBSUPERDERPY_VITA_TITLEID "DOS1RUKAV" CACHE INTERNAL "")

set(IMGTOWEBP_QUALITY "90" CACHE INTERNAL "")

set(EMSCRIPTEN_TOTAL_MEMORY "512" CACHE INTERNAL "")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/libsuperderpy/cmake")

include(libsuperderpy)

add_subdirectory(libsuperderpy)
add_subdirectory(src)
add_subdirectory(data)
