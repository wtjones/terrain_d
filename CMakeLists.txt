cmake_minimum_required(VERSION 3.21)

project(
    TerrainD
    VERSION 1.0
    DESCRIPTION "Terrain demo from 2003 that utilizes a quad tree for LOD. OpenGL + GLUT is used for rendering."
    LANGUAGES CXX)

add_executable(terrain_d
    terrain/cameratype.cpp
    terrain/frustumtype.cpp
    terrain/mygl.cpp
    terrain/terrainapp.cpp
    terrain/terraintype.cpp
    terrain/tgatype.cpp
    terrain/travmath.cpp)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLUT REQUIRED)

add_dependencies(terrain_d OpenGL::OpenGL)
include_directories(${GLUT_INCLUDE_DIRS} )

target_link_libraries(terrain_d OpenGL::OpenGL ${GLUT_LIBRARIES}  ${OPENGL_LIBRARIES} )