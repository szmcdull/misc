cmake_minimum_required (VERSION 2.6)
project (MiscWin)

set(libsrc INet.cpp)

add_library(objlib OBJECT ${libsrc})
add_library(objlib_static OBJECT ${libsrc})

add_library(MiscWinDll SHARED $<TARGET_OBJECTS:objlib>)
add_library(MiscWinLib STATIC $<TARGET_OBJECTS:objlib>)
add_library(MiscWinDllStatic SHARED $<TARGET_OBJECTS:objlib_static>)
add_library(MiscWinLibStatic STATIC $<TARGET_OBJECTS:objlib_static>)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set_target_properties(MiscWinDll MiscWinLib MiscWinDllStatic MiscWinLibStatic PROPERTIES DEBUG_POSTFIX "Debug")
    set_target_properties(MiscWinDll MiscWinLib MiscWinDllStatic MiscWinLibStatic PROPERTIES 
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib
    )
endif()

# shared libraries need PIC
set_property(TARGET objlib_static PROPERTY POSITION_INDEPENDENT_CODE 1)

set(CMAKE_CONFIGURATION_TYPES "Release;Debug")

target_compile_options(objlib_static PUBLIC "/MT$<$<CONFIG:Debug>:d>")

#string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASESTATICRUNTIME ${CMAKE_CXX_FLAGS_RELEASE})
#string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUGSTATICRUNTIME ${CMAKE_CXX_FLAGS_DEBUG})
#set(CMAKE_SHARED_LINKER_FLAGS_RELEASESTATICRUNTIME ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
#set(CMAKE_SHARED_LINKER_FLAGS_DEBUGSTATICRUNTIME ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})



# shared and static libraries built from the same object files
#add_library(MiscWin SHARED $<TARGET_OBJECTS:objlib>)
#add_library(MiscWinStatic STATIC $<TARGET_OBJECTS:objlib>)
