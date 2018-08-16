add_library(defragmenter.cpp defragmenter.cpp)
target_link_libraries(defragmenter.cpp linearprobing.cpp)

add_library(mynew.cpp mynew.cpp)

add_executable(DefragRunner.cpp DefragRunner.cpp)
target_link_libraries(DefragRunner.cpp defragmenter.cpp)
target_link_libraries(DefragRunner.cpp mynew.cpp)

add_library(linearprobing.cpp linearprobing.cpp)
target_link_libraries(linearprobing.cpp defragmenter.cpp)
target_link_libraries(linearprobing.cpp mynew.cpp)