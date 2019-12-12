cmake_minimum_required(VERSION 3.6)

set(lib_name recast)
set(target_name ext_${lib_name})

project(${lib_name})

set(${target_name}_src
  DebugUtils/DebugDraw.cpp
  DebugUtils/DetourDebugDraw.cpp
  Detour/DetourAlloc.cpp
  Detour/DetourCommon.cpp
  Detour/DetourNavMesh.cpp
  Detour/DetourNavMeshBuilder.cpp
  Detour/DetourNavMeshQuery.cpp
  Detour/DetourNode.cpp
  DetourCrowd/DetourCrowd.cpp
  DetourCrowd/DetourLocalBoundary.cpp
  DetourCrowd/DetourObstacleAvoidance.cpp
  DetourCrowd/DetourPathCorridor.cpp
  DetourCrowd/DetourPathQueue.cpp
  DetourCrowd/DetourProximityGrid.cpp
  DetourTileCache/DetourTileCache.cpp
  DetourTileCache/DetourTileCacheBuilder.cpp
  fastlz/fastlz.c
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC ..)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)
if(ANDROID)
    get_filename_component(LIB_PUBLISH_DIRECTORY
            "${CMAKE_SOURCE_DIR}/../../../build/built_libs/android/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}"
            ABSOLUTE
            )

    file(MAKE_DIRECTORY ${LIB_PUBLISH_DIRECTORY})


    add_custom_command(
            TARGET ext_recast POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            copy "${CMAKE_BINARY_DIR}/lib/libext_recast.a" "${LIB_PUBLISH_DIRECTORY}/libext_recast.a"
            COMMENT "Moving libext_recast.a to output directory"
    )
endif()