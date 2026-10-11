cmake_minimum_required(VERSION 4.1)
project(VisualiseTest DESCRIPTION "Visualisation library test suite.")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS_RELEASE "/permissive- /GS /W3 /WX /Zc:wchar_t /Gm- /Zc:inline /fp:precise /errorReport:prompt /Zc:forScope /std:c++20 /FC /EHsc /nologo /diagnostics:column /O2 /Zi /MD  /D \"NDEBUG\" /MP /Oi /Ot")
set(CMAKE_CXX_FLAGS_DEBUG   "/permissive- /GS /W3 /WX /Zc:wchar_t /Gm- /Zc:inline /fp:precise /errorReport:prompt /Zc:forScope /std:c++20 /FC /EHsc /nologo /diagnostics:column /Od /ZI /MDd /D \"_DEBUG\" /JMC")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/MANIFEST /NXCOMPAT /DYNAMICBASE /DEBUG:FULL /DLL /MACHINE:X64 /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /ERRORREPORT:PROMPT /NOLOGO /MANIFESTUAC:NO /LTCG:incremental /OPT:ICF /OPT:REF")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "/MANIFEST /NXCOMPAT /DYNAMICBASE /DEBUG:FULL /DLL /MACHINE:X64 /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /ERRORREPORT:PROMPT /NOLOGO /MANIFESTUAC:\"level='asInvoker' uiAccess='false'\" ")
message(STATUS "Project Source Directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.h" "${PROJECT_SOURCE_DIR}/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/Dependencies")
add_executable(VisualiseTest ${SOURCES})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(${PROJECT_SOURCE_DIR}/Dependencies/GTest/googletest-1.17.0 EXCLUDE_FROM_ALL)

target_include_directories(Visualise PUBLIC
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/Dependencies/Json/single_include/nlohmann
        ${PROJECT_SOURCE_DIR}/Dependencies/GTest/googletest-1.17.0/googletest/include
        ${PROJECT_SOURCE_DIR}/Dependencies/Eigen/eigen-5.0.0/Eigen
)
target_compile_definitions(Visualise PUBLIC
        OUTPUT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Output/"
)
target_link_libraries(VisualiseTest gtest gtest_main)
target_link_libraries(VisualiseTest Visualise)
