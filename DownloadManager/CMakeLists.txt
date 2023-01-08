cmake_minimum_required(VERSION 3.18)
project(DownloadManager)

set(CMAKE_CXX_STANDARD 20)

add_executable(DownloadManager main.cpp VlaknoObj.cpp VlaknoObj.h main.h)

target_link_libraries(DownloadManager pthread ssl crypto)#SSL a CRYPTO kvoli ssl

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak ")