cmake_minimum_required(VERSION 3.13)
set(PROJECT_NAME uvc)
project(${PROJECT_NAME} C CXX)

set(CMAKE_CXX_STANDARD 14)
add_definitions(-DNDEBUG -DCOMMIT_VERSION=":latest" -DCOMMIT_DIFF_SH="Schaudge King" -DCOMMIT_DIFF_FULL="") # -DNDEBUG is needed for paired calling (currently)
#add_definitions(-DUSE_STDLIB_THREAD)
add_compile_options(-std=c++14 -static-libstdc++ -Wall -fconcepts)

### add all required header files
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/CLI11-1.7.1)

# external dependencies to be discovered:
set(SAMTOOLS_DIR ${PROJECT_SOURCE_DIR}/ext/samtools)
set(HTSLIB_DIR ${PROJECT_SOURCE_DIR}/ext/htslib)
### discover samtools and htslib
find_library(BAM_ST_LIB NAMES libst.a HINTS ${SAMTOOLS_DIR})
find_library(HTS_LIB NAMES libhts.a HINTS ${HTSLIB_DIR})

### add all source file to the SRC_LIST *variable*
# omit

add_executable(${PROJECT_NAME} main.cpp CmdLineArgs.cpp common.cpp grouping.cpp iohts.cpp logging.cpp MolecularID.cpp version.cpp)
target_link_libraries(${PROJECT_NAME} -fopenmp ${HTS_LIB} pthread m z bz2 lzma curl crypto)

add_executable(debarcode debarcode_main.c)
target_link_libraries(debarcode ${HTS_LIB} pthread m z bz2 lzma curl crypto)

add_executable(integrate_haplotype integrate_haplotype_variation.cpp)
target_link_libraries(integrate_haplotype ${HTS_LIB} pthread m z bz2 lzma curl crypto)
