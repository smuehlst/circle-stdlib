cmake_minimum_required(VERSION 3.28)
project(FetchLLVM NONE)

# The tar file must be produced as Git release artifact with the
# circle-stdlib-release-tarball.yml workflow.
# Example: gh workflow run circle-stdlib-release-tarball.yml -R smuehlst/llvm-project -f tag=circle-stdlib-22.1.3-v2

include(FetchContent)
FetchContent_Declare(
    llvm_project
    URL                       https://github.com/smuehlst/llvm-project/releases/download/circle-stdlib-22.1.3-v2/llvm-project-circle-stdlib-22.1.3-v2.tar.gz
    URL_HASH                  SHA256=6cc9f09c22e6f73b80c70a60584dda55107e2628d690ee297f4ccab4e6f60497
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
    SOURCE_DIR                "${CMAKE_CURRENT_SOURCE_DIR}/../../libs/llvm-project"
    SOURCE_SUBDIR             cmake
)

FetchContent_MakeAvailable(llvm_project)
