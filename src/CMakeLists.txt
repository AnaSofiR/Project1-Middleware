file(GLOB_RECURSE SOURCES *.cpp)
add_executable(mom ${SOURCES})

add_executable(test_utils test_utils.cpp
    ../include/utils/Config.hpp ../src/utils/Config.cpp
    ../include/utils/Logger.hpp ../src/utils/Logger.cpp
)
target_include_directories(test_utils PRIVATE ../include)
