include(FetchContent)

add_subdirectory(glad)

message(STATUS "Downloading dependencies...")

find_package(glfw QUIET NAMES glfw glfw3)
if(NOT glfw_FOUND)
  message(STATUS "Downloading glfw3")
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY	https://github.com/glfw/glfw.git
    GIT_TAG 	master
  )
  FetchContent_MakeAvailable(glfw)
endif()

message(STATUS "Downloading nlohmann json")
FetchContent_Declare(
	json
	GIT_REPOSITORY	https://github.com/nlohmann/json.git
	GIT_TAG 	master
)
FetchContent_MakeAvailable(json)

message(STATUS "Downloading glm")
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	master
)
FetchContent_MakeAvailable(glm)

message(STATUS "Downloading dlib")
FetchContent_Declare(
	dlib
	GIT_REPOSITORY	https://github.com/davisking/dlib.git
	GIT_TAG 	master
)
FetchContent_MakeAvailable(dlib)
target_compile_options(dlib PRIVATE -w)


