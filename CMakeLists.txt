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

# The dylib defines the Objective-C classes of Block objects in the slices with the modern Objective-C runtime
# (data.m). The linker names the class symbols as the _NSConcrete*Block symbols that compiled Blocks use as their
# isa (Blocks.alias). The slices with the legacy runtime (i386) have no Objective-C classes here (see
# blocks_objc.h), so the alias list applies to the other slices only.
set(blocks_modern_objc_archs "")
foreach(arch IN LISTS DARLING_ARCHS)
	darling_arch_objc_abi(${arch} objc_abi)
	if (objc_abi STREQUAL "modern")
		list(APPEND blocks_modern_objc_archs ${arch})
	endif()
endforeach()
darling_xarch_flags(blocks_alias_flags ARCHS ${blocks_modern_objc_archs}
	OPTIONS "-Wl,-alias_list,${CMAKE_CURRENT_SOURCE_DIR}/Blocks.alias")
foreach(target system_blocks system_blocks_firstpass)
	set_property(TARGET ${target} APPEND_STRING PROPERTY LINK_FLAGS " ${blocks_alias_flags}")
endforeach()

# The static library is linked into code that has no Objective-C runtime (dyld), so it has no classes.
add_darling_static_library(system_blocks_static SOURCES ${blocks_sources})
set_property(TARGET system_blocks_static APPEND_STRING PROPERTY
	COMPILE_FLAGS " -fPIC -fno-stack-check -fno-stack-protector -DHAVE_OBJC=0")
#target_link_libraries(system_blocks PRIVATE system_kernel system_m system_malloc)

#make_fat(system_blocks)
make_fat(system_blocks_static)

install(TARGETS system_blocks DESTINATION libexec/darling/usr/lib/system)
