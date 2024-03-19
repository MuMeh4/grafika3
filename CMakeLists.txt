cmake_minimum_required(VERSION 3.27)
project(GrafikaHaziCsomag)

set(CMAKE_CXX_STANDARD 14)

find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(Libs)
include_directories(Libs/freeglut)
include_directories(Libs/freeglut/include)
include_directories(Libs/freeglut/include/GL)
include_directories(Libs/glew-1.13.0)
include_directories(Libs/glew-1.13.0/include)
include_directories(Libs/glew-1.13.0/include/GL)
include_directories(Programs)
include_directories(Programs/Skeleton)
include_directories(Programs/Skeleton/Skeleton)

add_executable(GrafikaHaziCsomag
        Libs/freeglut/include/GL/freeglut.h
        Libs/freeglut/include/GL/freeglut_ext.h
        Libs/freeglut/include/GL/freeglut_std.h
        Libs/freeglut/include/GL/glut.h
        Libs/glew-1.13.0/include/GL/glew.h
        Libs/glew-1.13.0/include/GL/glxew.h
        Libs/glew-1.13.0/include/GL/wglew.h
        Programs/Skeleton/Skeleton/framework.cpp
        Programs/Skeleton/Skeleton/framework.h
        Programs/Skeleton/Skeleton/Skeleton.cpp)

target_link_libraries(GrafikaHaziCsomag ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})