cmake_minimum_required(VERSION 3.22)
project(SimplePhongRender
    DESCRIPTION "A simple phong renderer"
    HOMEPAGE_URL "https://github.com/cedmundo/SimplePhoneRender"
    VERSION "1.0.0"
    LANGUAGES C CXX
)

# Global configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_definitions("-Wall" "-g")

# Add other generic libraries
# ...

# Add subdirectories
add_subdirectory(vendor)

# Add internal libraries
add_subdirectory(xmath)

# Add main executable
add_executable(SimplePhongRender)
target_sources(SimplePhongRender
  INTERFACE core.h camera.h model.h
  PRIVATE core.c camera.c model.c main.c
)
target_link_libraries(SimplePhongRender glfw glad cgltf ktx_read xmath)

# Copy assets dir
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")
if(EXISTS "${ASSETS_DIR}")
  add_custom_command(
    TARGET SimplePhongRender PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/assets")

  add_custom_command(
    TARGET SimplePhongRender PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif()

