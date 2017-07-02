cmake_minimum_required(VERSION 3.6)
project(codewars)

set(FREEIMAGE_INCLUDE C:/Users/martin/Documents/Projects/C++/FreeImage/Source)
set(ASSIMP_INCLUDE C:/Users/Public/Documents/Programming/Libraries/assimp-3.3.1/include)
set(ASSIMP_LIBD C:/Users/Public/Documents/Programming/Libraries/assimp-3.3.1/build/code/Debug)
set(ASSIMP_LIB C:/Users/Public/Documents/Programming/Libraries/assimp-3.3.1/build/code/Release)

set(FREEIMAGE_LIB C:/Users/martin/Documents/Projects/C++/FreeImage/Dist/x32)
set(PYTHON_LIB c:/Python36-32/libs)
set(PYTHON_DLL c:/Python36-32)


include_directories(engine/include games/spacefight/include)
include_directories(${FREEIMAGE_INCLUDE})
include_directories(${ASSIMP_INCLUDE})
include_directories(C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/ucrt)
include_directories(C:/Program Files (x86)/Windows Kits/10/Include/10.0.14393.0/um)
include_directories(C:/Python36-32/include)

#link_directories(${PYTHON_LIB} ${ASSIMP_LIB} ${FREEIMAGE_LIB} )

file(GLOB engine_sources engine/src/*.cpp)
file(GLOB game_sources games/spacefight/src/*.cpp)


add_executable(engine_main WIN32 ${engine_sources} ${game_sources})
target_link_libraries(engine_main debug ${ASSIMP_LIBD}/assimp-vc120-mt.lib 
								  optimized ${ASSIMP_LIB}/assimp-vc120-mt.lib 
								  debug ${FREEIMAGE_LIB}/FreeImaged.lib 
								  optimized ${FREEIMAGE_LIB}/FreeImage.lib 
								  dxgi.lib dxguid.lib d3d11.lib dinput8.lib d3dcompiler.lib kernel32.lib 
								  debug ${PYTHON_LIB}/python36_d.lib
								  optimized ${PYTHON_LIB}/python36.lib)

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${ASSIMP_LIB}/assimp-vc2017-mt.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>)    

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PYTHON_DLL}/python36_d.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>)   

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PYTHON_DLL}/python36.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>)  

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${FREEIMAGE_LIB}/FreeImaged.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>)   

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${FREEIMAGE_LIB}/FreeImage.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>)  

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/engine/shaders"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>/shaders)  

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/engine/textures"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>/textures)  

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/engine/models"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>/models) 
		
add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/games/spacefight/models"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>/games/spacefight/models)  

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/games/spacefight/textures"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>/games/spacefight/textures)  

add_custom_command(TARGET engine_main POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/games/spacefight/sound"      # <--this is in-file
        $<TARGET_FILE_DIR:engine_main>/games/spacefight/sound)  

# TODO add copying of shader/model/texture resources