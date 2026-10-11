cmake_minimum_required(VERSION 4.1)
project(Visualise DESCRIPTION "Visualisation library for C++")

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
list(FILTER SOURCES EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/Python")

add_library(Visualise STATIC ${SOURCES}
        C++/IO/BIN.h
        C++/types.h
        C++/system.h
        C++/visualise_3D.h
        C++/visualise_2D.h
        C++/visualise_base.h)

target_include_directories(Visualise PUBLIC
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/Dependencies/Json/single_include/nlohmann
)
target_link_libraries(Visualise)

set(PYTHON_MAIN_DIR "${PROJECT_SOURCE_DIR}/Python/src/")
set(PYTHON_EXE_DIR "${PROJECT_SOURCE_DIR}/Python/.venv/Scripts/")
set(PYTHON_DIST_DIR "${PROJECT_SOURCE_DIR}/Python/dist/")
target_compile_definitions(Visualise PUBLIC
        PYTHON_EXE_DIR="${PYTHON_EXE_DIR}"
        PYTHON_MAIN_DIR="${PYTHON_MAIN_DIR}"
)

add_custom_target(VisualiseExecutable
        COMMAND "${PYTHON_EXE_DIR}/python.exe" -m PyInstaller
                --onedir
                --name Visualise
                --distpath "${PYTHON_DIST_DIR}"
                "${PYTHON_MAIN_DIR}/main.py"
)