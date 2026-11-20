project(system_blocks)

cmake_minimum_required(VERSION 3.13)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include(darling_static_lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdinc")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib -Wl,-flat_namespace -Wl,-undefined,suppress")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DHAVE_STDINT_H=1)

set(blocks_sources
	data.c
	data.m
	runtime.cpp
)

set(DYLIB_INSTALL_NAME "/usr/lib/system/libsystem_blocks.dylib")
add_circular(system_blocks FAT
	SOURCES
		${blocks_sources}
	SIBLINGS
		system_kernel
		system_malloc
		system_pthread
		platform
		system_c
#	UPWARD
#		objc_firstpass
)

# The dylib defines the Objective-C classes of Block objects in its 64-bit slices (data.m). The linker names the
# class symbols as the _NSConcrete*Block symbols that compiled Blocks use as their isa (Blocks.alias). The
# 32-bit slice has no Objective-C classes here (see blocks_objc.h), so the alias list applies to the 64-bit
# slice only.
foreach(target system_blocks system_blocks_firstpass)
	set_property(TARGET ${target} APPEND_STRING PROPERTY
		LINK_FLAGS " -Xarch_${APPLE_ARCH_64BIT} -Wl,-alias_list,${CMAKE_CURRENT_SOURCE_DIR}/Blocks.alias")
endforeach()

# The static library is linked into code that has no Objective-C runtime (dyld), so it has no classes.
add_darling_static_library(system_blocks_static SOURCES ${blocks_sources})
set_property(TARGET system_blocks_static APPEND_STRING PROPERTY
	COMPILE_FLAGS " -fPIC -fno-stack-check -fno-stack-protector -DHAVE_OBJC=0")
#target_link_libraries(system_blocks PRIVATE system_kernel system_m system_malloc)

#make_fat(system_blocks)
make_fat(system_blocks_static)

install(TARGETS system_blocks DESTINATION libexec/darling/usr/lib/system)
