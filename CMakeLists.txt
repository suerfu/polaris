cmake_minimum_required( VERSION 3.2 )

if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project( polaris VERSION 2.1.0 )


SET( CMAKE_CXX_STANDARD 11)
SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -std=c++11 -lpthread -ldl" )
#find_package( Threads )


include_directories( ./include )
file( GLOB INCLUDES "include/*.h" )
file( GLOB SOURCES "src/*.cpp" )


add_library( polaris SHARED ${SOURCES} )
install( TARGETS polaris EXPORT polarisTargets )
install( FILES ${includes} DESTINATION polaris/include )
install( EXPORT polarisTargets FILE polaris-config.cmake DESTINATION lib/cmake/polaris )


add_executable( polaris-bin exe/polaris.cpp ${SOURCES})
#target_link_libraries( polaris Threads::Threads )
set_target_properties( polaris-bin PROPERTIES OUTPUT_NAME polaris )
install( TARGETS polaris-bin )

