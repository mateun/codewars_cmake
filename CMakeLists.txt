cmake_minimum_required(VERSION 3.6)
project(codewars)

set(FREEIMAGE_INCLUDE E:/Projects/C++/FreeImage/Source)
set(FREEIMAGE_LIB e:/Projects/C++/FreeImage/Dist/x64)


include_directories(engine/include games/src/**/src/*.h)
include_directories(${FREEIMAGE_INCLUDE})

#link_directories(${PYTHON_LIB} ${ASSIMP_LIB} ${FREEIMAGE_LIB} )

file(GLOB engine_sources engine/src/*.cpp)
file(GLOB game_sources games/src/koenig/src/*.cpp)

add_executable(engine_main WIN32 ${engine_sources} ${game_sources} engine/include/engine_init.cpp)
#target_link_libraries(engine_main debug ${ASSIMP_LIBD}/assimp-vc140-mt.lib
#								  optimized ${ASSIMP_LIB}/assimp-vc140-mt.lib
#								  debug ${FREEIMAGE_LIB}/FreeImaged.lib
#								  optimized ${FREEIMAGE_LIB}/FreeImage.lib
#								  dxgi.lib dxguid.lib d3d11.lib dinput8.lib d3dcompiler.lib kernel32.lib
#								  debug ${PYTHON_LIB}/python36_d.lib
#								  optimized ${PYTHON_LIB}/python36.lib)

target_link_libraries(engine_main
						${FREEIMAGE_LIB}/FreeImage.lib
						dxgi.lib dxguid.lib d3d11.lib dinput8.lib d3dcompiler.lib kernel32.lib)

#add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#        "${ASSIMP_LIBD}/assimp-vc140-mt.dll"      # <--this is in-file
#        $<TARGET_FILE_DIR:engine_main>)

#add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#        "${PYTHON_DLL}/python36_d.dll"      # <--this is in-file
#        $<TARGET_FILE_DIR:engine_main>)

#add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#        "${PYTHON_DLL}/python36.dll"      # <--this is in-file
#        $<TARGET_FILE_DIR:engine_main>)

#add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#        "${FREEIMAGE_LIB}/FreeImaged.dll"      # <--this is in-file
#        $<TARGET_FILE_DIR:engine_main>)
#
#add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#        "${FREEIMAGE_LIB}/FreeImage.dll"      # <--this is in-file
#        $<TARGET_FILE_DIR:engine_main>)
#
#		add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
#        "${PROJECT_SOURCE_DIR}/engine/textures"      # <--this is in-file
#        $<TARGET_FILE_DIR:engine_main>/textures)
#
#add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
#    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
#        "${PROJECT_SOURCE_DIR}/games/assets"      # <--this is in-file
#        $<TARGET_FILE_DIR:engine_main>/games/assets)

# TODO add copying of shader/model/texture resources