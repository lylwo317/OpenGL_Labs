cmake_minimum_required(VERSION 3.10)
project(OpenGL_Labs)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)
# note that the order is important for setting the libs
# use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype ${ASSIMP_LIBRARY} SDL2 EGL m)
set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

set(CHAPTERS
        test
        1.getting_started
#        2.lighting
#        3.model_loading
#        4.advanced_opengl
#        5.advanced_lighting
#        6.pbr
#        7.in_practice
        )

set(test
        1.gles_linux)

set(1.getting_started
        1.1.hello_window
#        1.2.hello_window_clear
        2.1.hello_triangle
        2.2.hello_triangle_indexed
#        2.3.hello_triangle_exercise1
#        2.4.hello_triangle_exercise2
#        2.5.hello_triangle_exercise3
#        3.1.shaders_uniform
#        3.2.shaders_interpolation
#        3.3.shaders_class
#        4.1.textures
#        4.2.textures_combined
#        4.4.textures_exercise2
#        4.5.textures_exercise3
#        4.6.textures_exercise4
#        5.1.transformations
#        5.2.transformations_exercise2
#        6.1.coordinate_systems
#        6.2.coordinate_systems_depth
#        6.3.coordinate_systems_multiple
#        7.1.camera_circle
#        7.2.camera_keyboard_dt
#        7.3.camera_mouse_zoom
#        7.4.camera_class
        )

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

# then create a project file per tutorial
foreach(CHAPTER ${CHAPTERS})
    foreach(DEMO ${${CHAPTER}})
        file(GLOB SOURCE
                "src/${CHAPTER}/${DEMO}/*.h"
                "src/${CHAPTER}/${DEMO}/*.cpp"
                "src/${CHAPTER}/${DEMO}/*.c"
                "src/${CHAPTER}/${DEMO}/*.vs"
                "src/${CHAPTER}/${DEMO}/*.fs"
                "src/${CHAPTER}/${DEMO}/*.gs"
                )
        set(NAME "${CHAPTER}__${DEMO}")
        add_executable(${NAME} ${SOURCE})
        target_link_libraries(${NAME} ${LIBS})
        if(WIN32)
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
            set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/Debug")
        elseif(UNIX AND NOT APPLE)
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
        elseif(APPLE)
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
            set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${CHAPTER}")
        endif(WIN32)
        # copy shader files to build directory
        file(GLOB SHADERS
                "src/${CHAPTER}/${DEMO}/*.vs"
                # "src/${CHAPTER}/${DEMO}/*.frag"
                "src/${CHAPTER}/${DEMO}/*.fs"
                "src/${CHAPTER}/${DEMO}/*.gs"
                )
        foreach(SHADER ${SHADERS})
            if(WIN32)
                # configure_file(${SHADER} "test")
                add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
            elseif(UNIX AND NOT APPLE)
                file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/${CHAPTER})
            elseif(APPLE)
                # create symbolic link for *.vs *.fs *.gs
                get_filename_component(SHADERNAME ${SHADER} NAME)
                makeLink(${SHADER} ${CMAKE_SOURCE_DIR}/bin/${CHAPTER}/${SHADERNAME} ${NAME})
            endif(WIN32)
        endforeach(SHADER)
        # if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
        if(MSVC)
            configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
        endif(MSVC)
    endforeach(DEMO)
endforeach(CHAPTER)

include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(/usr/include/SDL2/)
