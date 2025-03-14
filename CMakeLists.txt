cmake_minimum_required(VERSION 3.10)
project(polaris)

set(SOMAJOR 2)
set(SOMINOR 1)
set(LIBNAME "lib${PROJECT_NAME}.so.${SOMAJOR}.${SOMINOR}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall")

# Include directories
include_directories(include)

# Collect source files
file(GLOB CPP_FILES "src/*.cpp")

# Create library
add_library(${PROJECT_NAME} SHARED ${CPP_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    VERSION "${SOMAJOR}.${SOMINOR}"
    SOVERSION "${SOMAJOR}"
)

# Linker flags
target_link_libraries(${PROJECT_NAME} pthread dl)

# Build the executable
add_executable(${PROJECT_NAME}_exec exe/${PROJECT_NAME}.cpp)
target_link_libraries(${PROJECT_NAME}_exec ${PROJECT_NAME})

# Installation prefix
set(INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Installation directory prefix")

# Installation
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_exec
    RUNTIME DESTINATION ${INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${INSTALL_PREFIX}/lib
)
install(DIRECTORY include/ DESTINATION ${INSTALL_PREFIX}/include/${PROJECT_NAME})

# Create symbolic links for library versions
install(CODE "execute_process(COMMAND ln -sf ${INSTALL_PREFIX}/lib/lib${PROJECT_NAME}.so.${SOMAJOR}.${SOMINOR} ${INSTALL_PREFIX}/lib/lib${PROJECT_NAME}.so.${SOMAJOR})")
install(CODE "execute_process(COMMAND ln -sf ${INSTALL_PREFIX}/lib/lib${PROJECT_NAME}.so.${SOMAJOR} ${INSTALL_PREFIX}/lib/lib${PROJECT_NAME}.so)")

# Uninstall target
add_custom_target(uninstall
    COMMAND xargs rm < install_manifest.txt
    COMMAND rm -rf ${INSTALL_PREFIX}/include/${PROJECT_NAME}
    VERBATIM
)

# Clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/Makefile
    COMMAND rm -rf ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND rm -rf ${PROJECT_NAME}_exec
    VERBATIM
)

