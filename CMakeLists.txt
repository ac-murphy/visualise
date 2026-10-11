cmake_minimum_required(VERSION 4.1)
project(Visualise)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(Visualise)
add_subdirectory(VisualiseTest)

target_compile_definitions(Visualise PUBLIC
        VISUALISE_PYTHON_EXE_DIR="${PROJECT_SOURCE_DIR}/.venv/Scripts/"
        VISUALISE_OUTPUT_DIR="${CMAKE_CURRENT_SOURCE_DIR}/VisualiseTest/Output/"
)
