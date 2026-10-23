cmake_minimum_required(VERSION 3.24)
project(RAYLIB_TEMPLATE)
set(CMAKE_CXX_STANDARD 23)
# Toggle to set whether this is a prod build (ON) or debug (OFF)
set(PRODUCTION_BUILD OFF CACHE BOOL "Make this a production build" FORCE)

#####################################
##### STATIC LINK RUNTIME LIBS ######
#####################################

# Statically links the c and c++ runtime libraries. This is needed for game development, if the binaries
# are to be distributed on windows, as dynamically linking the c and c++ runtime libraries is sketchy and
# very version dependent on windows.
if(MSVC)
       set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
       set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
       add_compile_options(/arch:AVX2) # Ensures SIMD optimizations. Remove if triggers errors on apple device
else()
       add_link_options(-static-libstdc++ -static-libgcc)
       add_compile_options(-mavx2 -mfma) # Ensures SIMD optimizations. Remove if triggers errors on apple device
endif()


############################
##### LIBRARY OPTIONS ######
############################

# -- Set Library options --
# Makes raylib build as a static library, rather than a shared library
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Raylib as a static library" FORCE)

######################
##### LIBRARIES ######
######################

# -- Add Libraries --
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/raylib-6.0)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/rlImGui-Raylib_6_0)

# AS imgui has no cmake, it must be compiled and statically linked manually
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-1.92.7")
add_library(imgui_lib STATIC
       ${IMGUI_DIR}/imgui.cpp
       ${IMGUI_DIR}/imgui_draw.cpp
       ${IMGUI_DIR}/imgui_demo.cpp
       ${IMGUI_DIR}/imgui_tables.cpp
       ${IMGUI_DIR}/imgui_widgets.cpp
       ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
       ${IMGUI_DIR}/backends/imgui_impl_sdlgpu3.cpp
        # Add/remove backends to match what you actually use
)
target_include_directories(imgui_lib PUBLIC
       ${IMGUI_DIR}
       ${IMGUI_DIR}/backends
)

########################################
##### EXECUTABLE INCLUDES AND SRC ######
########################################

# --- Your executable ---
#Bundles togetehr all cpp files in src recursively, so they can be added as an executable all as one
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(${CMAKE_PROJECT_NAME}
    "${MY_SOURCES}"
)

# -- Include headers --
#Path for include headers
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")


####################################
##### PREPROCESSOR DIRECTIVES ######
####################################

# -- Preprocessor directives creation --
if (PRODUCTION_BUILD)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="./resources/")
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC PRODUCTION_BUILD=1)
else()
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC PRODUCTION_BUILD=0)
endif()
# Creates the RESOURCES_PATH preprocessor macro which directs all file loading operations to the resources file


############################
##### DISABLE CONSOLE ######
############################

if(MSVC) # If using the VS compiler...
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

	if(PRODUCTION_BUILD)
		#YOU CAN REMOVE THE CONSOLE WITH THIS LINE!
		set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console
	endif()

elseif(MINGW) # Windows / MinGW GCC/Clang
	if(PRODUCTION_BUILD)
		# Remove console on Windows under MinGW
		target_link_options("${CMAKE_PROJECT_NAME}" PRIVATE
			-mwindows
		)
	endif()

elseif(APPLE OR UNIX) # macOS / Linux
	if(PRODUCTION_BUILD)
		target_link_options("${CMAKE_PROJECT_NAME}" PRIVATE -s)
	endif()
endif()

####################
##### LINKING ######
####################

# -- Final linking step --
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    imgui_lib
    raylib_static
    rlimgui
)
