cmake_minimum_required(VERSION 3.28)
project(FetchLLVM NONE)

include(FetchContent)
FetchContent_Declare(
    llvm_project
    URL                       https://github.com/smuehlst/llvm-project/archive/refs/tags/circle-stdlib-22.1.3-v2.tar.gz
    URL_HASH                  SHA256=8d5f7391ef9a2bdcfc2a93fcc6e9b16fb267d31d152f1f020fcdf207b8488f55
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
    SOURCE_DIR                "${CMAKE_CURRENT_SOURCE_DIR}/../../libs/llvm-project"
    SOURCE_SUBDIR             cmake
)

FetchContent_MakeAvailable(llvm_project)
