project(system_blocks)

cmake_minimum_required(VERSION 2.4.0)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include(darling_static_lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdinc")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib -Wl,-flat_namespace -Wl,-undefined,suppress")

include_directories(${CMAKE_SOURCE_DIR}/src/libc/darwin
	${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DTARGET_OS_MAC=1)
add_definitions(-DHAVE_STDINT_H=1)
add_definitions(-D__APPLE__ -D__DYNAMIC__)

set(blocks_sources
	data.c
	runtime.c
)

set(CMAKE_AR "${CMAKE_BINARY_DIR}/src/external/cctools-port/cctools/ar/x86_64-apple-darwin11-ar" PARENT_SCOPE)
set(CMAKE_RANLIB "${CMAKE_BINARY_DIR}/src/external/cctools-port/cctools/misc/ranlib" PARENT_SCOPE)

set(DYLIB_INSTALL_NAME "/usr/lib/system/libsystem_blocks.dylib")
add_circular(system_blocks FAT
	SOURCES
		${blocks_sources}
	SIBLINGS
		system_kernel
		system_malloc
		system_pthread
		system_c
#	UPWARD
#		objc_firstpass
)
add_library(system_blocks_static STATIC ${blocks_sources})
#target_link_libraries(system_blocks PRIVATE system_kernel system_m system_malloc)
add_dependencies(system_blocks_static ranlib x86_64-apple-darwin11-ar)

#make_fat(system_blocks)
make_fat(system_blocks_static)

install(TARGETS system_blocks DESTINATION libexec/darling/usr/lib/system)
