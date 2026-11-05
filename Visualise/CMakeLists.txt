cmake_minimum_required(VERSION 4.1)
project(Visualise DESCRIPTION "Visualisation library for C++")

# Set CMake project options.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Add sources (and exclude dependencies).
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.h" "${PROJECT_SOURCE_DIR}/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/Dependencies")
list(FILTER SOURCES EXCLUDE REGEX "${PROJECT_SOURCE_DIR}/Python")
add_library(Visualise STATIC ${SOURCES})

# Set CMake Python options.
option(PYTHON_TEST_MODE "Run Python with source files" ON)
set(PYTHON_SOURCE_DIR "${PROJECT_SOURCE_DIR}/Python/src/")
set(PYTHON_DIST_DIR "${PROJECT_SOURCE_DIR}/Python/dist/")
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(PYTHON_EXE_PATH "${PROJECT_SOURCE_DIR}/Python/.venv/Scripts/python.exe")
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(PYTHON_EXE_PATH "${PROJECT_SOURCE_DIR}/Python/.venv/bin/python")
endif()

# Set CMake Blender options.
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(BLENDER_EXE_PATH "blender")
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(BLENDER_EXE_PATH "/opt/blender/blender")
endif()

# Build info.
message(STATUS "Project Source Directory: ${PROJECT_SOURCE_DIR}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Running Python with source files: ${PYTHON_TEST_MODE}")

# Define CMake variables.
target_compile_definitions(Visualise PUBLIC
        PYTHON_EXE_PATH="${PYTHON_EXE_PATH}"
        PYTHON_DIR="${PROJECT_SOURCE_DIR}/Python/"
        PYTHON_SOURCE_DIR="${PYTHON_SOURCE_DIR}"
        PYTHON_DIST_DIR="${PYTHON_DIST_DIR}"
        PYTHON_TEST_MODE=$<BOOL:${PYTHON_TEST_MODE}>

        BLENDER_EXE_PATH="${BLENDER_EXE_PATH}"

        SETTINGS_DIR="${PROJECT_SOURCE_DIR}/Settings/"
)

# Specify include paths.
target_include_directories(Visualise PUBLIC
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/C++/
        ${PROJECT_SOURCE_DIR}/Dependencies/Json/single_include/nlohmann
)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    # Compiler specific flags.
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_RELEASE "/permissive- /GS /W3 /WX /Zc:wchar_t /Gm- /Zc:inline /fp:precise /errorReport:prompt /Zc:forScope /std:c++20 /FC /EHsc /nologo /diagnostics:column /O2 /Zi /MD  /D \"NDEBUG\" /MP /Oi /Ot")
        set(CMAKE_CXX_FLAGS_DEBUG   "/permissive- /GS /W3 /WX /Zc:wchar_t /Gm- /Zc:inline /fp:precise /errorReport:prompt /Zc:forScope /std:c++20 /FC /EHsc /nologo /diagnostics:column /Od /ZI /MDd /D \"_DEBUG\" /JMC")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/MANIFEST /NXCOMPAT /DYNAMICBASE /DEBUG:FULL /DLL /MACHINE:X64 /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /ERRORREPORT:PROMPT /NOLOGO /MANIFESTUAC:NO /LTCG:incremental /OPT:ICF /OPT:REF")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "/MANIFEST /NXCOMPAT /DYNAMICBASE /DEBUG:FULL /DLL /MACHINE:X64 /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /ERRORREPORT:PROMPT /NOLOGO /MANIFESTUAC:\"level='asInvoker' uiAccess='false'\" ")
    endif()

    # Target to generate Python executable.
    add_custom_target(VisualiseExecutable
            COMMAND "${PYTHON_EXE_PATH}" -m PyInstaller
            --onedir
            --name Visualise
            --distpath "${PYTHON_DIST_DIR}"
            --noconfirm
            "${PYTHON_SOURCE_DIR}\\main.py"
    )
endif()


if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    # Compiler specific flags.
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS_RELEASE "-std=c++20 -ggdb3 -O3 -fallow-store-data-races -fno-semantic-interposition -fno-math-errno -funsafe-math-optimizations -fno-rounding-math -fexcess-precision=fast -Wall -Wno-reorder -Wno-unused-function -ftemplate-backtrace-limit=0 -fmessage-length=0")
        set(CMAKE_CXX_FLAGS_DEBUG   "-std=c++20 -ggdb3 -DDEBUG -Wall -Wno-reorder -Wno-unused-function -ftemplate-backtrace-limit=0 -fmessage-length=0")
        set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-Ofast -W -Wno-as-needed" )
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG   "-W -Wno-as-needed" )
    endif()
endif()