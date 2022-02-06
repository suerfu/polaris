cmake_minimum_required( VERSION 3.2 )

project( polaris )

SET( CMAKE_CXX_STANDARD 11)
SET( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -std=c++11 -lpthread -ldl" )
#find_package( Threads )

include_directories( ./include )
file( GLOB SOURCES "src/*.cpp" )

add_library( polaris SHARED ${SOURCES} )
install( TARGETS polaris )

add_executable( polaris-bin exe/polaris.cpp ${SOURCES})
#target_link_libraries( polaris Threads::Threads )
set_target_properties( polaris-bin PROPERTIES OUTPUT_NAME polaris )
install( TARGETS polaris-bin )

