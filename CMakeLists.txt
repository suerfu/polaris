cmake_minimum_required(VERSION 3.10)
project(polaris)

# Add /usr/local/lib/cmake to the CMAKE_PREFIX_PATH
list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib/cmake")

set(SOMAJOR 2)
set(SOMINOR 1)
set(LIBNAME "${PROJECT_NAME}.so.${SOMAJOR}.${SOMINOR}")

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
add_executable(${PROJECT_NAME}_bin exe/${PROJECT_NAME}.cpp)

set_target_properties(${PROJECT_NAME}_bin PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    )

target_link_libraries(${PROJECT_NAME}_bin ${PROJECT_NAME})

# Installation prefix
set(INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH "Installation directory prefix")

# Export the targets to a file
install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_bin
    EXPORT polarisTargets
    RUNTIME DESTINATION ${INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${INSTALL_PREFIX}/lib
)
install(DIRECTORY include/ DESTINATION ${INSTALL_PREFIX}/include/${PROJECT_NAME})

# Install the export file
install(EXPORT polarisTargets
    FILE polarisTargets.cmake
    NAMESPACE polaris::
    DESTINATION lib/cmake/polaris
)

# Create and install polarisConfig.cmake file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/polarisConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/polarisConfig.cmake"
    INSTALL_DESTINATION lib/cmake/polaris
)

# Install the configuration file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/polarisConfig.cmake"
    DESTINATION lib/cmake/polaris
)

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
