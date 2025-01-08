cmake_minimum_required(VERSION 3.12)
project(EscapeDungeon)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)

# pre-define
add_definitions(-DDEBUG -DPHYSIC_IMPL)

# include & link
include_directories(
	SYSTEM "${CMAKE_SOURCE_DIR}/thirdparty/include"
	"${CMAKE_SOURCE_DIR}/src"
)
link_directories(
	SYSTEM "${CMAKE_SOURCE_DIR}/thirdparty/lib"
)

# execute file
file(GLOB_RECURSE SRC_INNER ${CMAKE_SOURCE_DIR}/src/*.cpp)
message(STATUS "Source files: ${SRC_INNER}")

add_executable(EscapeDungeon 
	main.cpp 
	glad.c
	${SRC_INNER}
)
#add_executable(AgedTest
#	main_deprecated.cpp 
#	glad.c
#	${SRC_INNER}
#)

# libs
file(GLOB LIBS "${CMAKE_SOURCE_DIR}/thirdparty/lib/*.lib")
target_link_libraries(EscapeDungeon ${LIBS})
#target_link_libraries(AgedTest ${LIBS})

# assets
file(GLOB ASSETS "${CMAKE_SOURCE_DIR}/assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

# dlls
file(GLOB DLLS "${CMAKE_SOURCE_DIR}/thirdparty/dll/*.dll")
file(COPY ${DLLS} DESTINATION ${CMAKE_BINARY_DIR})
#file(GLOB PDBS "${CMAKE_SOURCE_DIR}/thirdparty/dll/*.pdb")
#file(COPY ${PDBS} DESTINATION ${CMAKE_BINARY_DIR})
