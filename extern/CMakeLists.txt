add_subdirectory(glad)

# find_package(OpenGL REQUIRED)

find_package(glfw QUIET NAMES glfw glfw3)
if(NOT glfw_FOUND)
  add_subdirectory(glfw)
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

add_subdirectory(glm)

add_subdirectory(dlib)



