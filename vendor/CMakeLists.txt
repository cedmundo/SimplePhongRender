# GLFW
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared library")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Disable examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Disable tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Disable docs")
set(GLFW_INSTALL OFF CACHE BOOL "Disable install")
add_subdirectory(glfw)

# GLAD
add_subdirectory(glad)

# cGLTF
add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE cgltf)

# LibKTX2
set(KTX_FEATURE_STATIC_LIBRARY ON CACHE BOOL "Enable static lib" FORCE)
set(KTX_FEATURE_DOC OFF CACHE BOOL "Disable docs" FORCE)
set(KTX_FEATURE_JNI OFF CACHE BOOL "Disable JNI" FORCE)
set(KTX_FEATURE_PY OFF CACHE BOOL "Disable PY" FORCE)
set(KTX_FEATURE_TESTS OFF CACHE BOOL "Disable tests" FORCE)
set(KTX_FEATURE_TOOLS OFF CACHE BOOL "Disable tests" FORCE)
set(KTX_FEATURE_TOOLS_CTS OFF CACHE BOOL "Disable tools CTS" FORCE)
set(KTX_FEATURE_KTX1 ON CACHE BOOL "Enable KTX1" FORCE)
set(KTX_FEATURE_KTX2 ON CACHE BOOL "Enable KTX2" FORCE)
set(KTX_FEATURE_GL_UPLOAD ON CACHE BOOL "Enable OpenGL upload" FORCE)
set(KTX_FEATURE_VK_UPLOAD OFF CACHE BOOL "Disable Vulkan upload" FORCE)
add_subdirectory(ktx)
