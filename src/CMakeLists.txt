include_directories(include/)

# file(COPY ../models DESTINATION ${CMAKE_BINARY_DIR})
# file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})


SET(VANCHE_SOURCES
  renderer/window.c
  renderer/vmodelRenderer.cpp
  loader/loader.cpp
  face_pose_detection/face_mapper.cpp)


IF(DEFINED VANCHE_dlib68)
  SET(VANCHE_SOURCES ${VANCHE_SOURCES} face_pose_detection/face_recognizer_dlib68.cpp)
ELSE()
  add_definitions(-DVANCHE_dlib68)
  SET(VANCHE_SOURCES ${VANCHE_SOURCES} face_pose_detection/face_recognizer_dlib68.cpp)
ENDIF()

add_executable(vanche main.cpp ${VANCHE_SOURCES})

target_link_libraries(vanche PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(vanche PRIVATE glad)
target_link_libraries(vanche PRIVATE glfw)
target_link_libraries(vanche PRIVATE glm::glm)
target_link_libraries(vanche PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vanche PRIVATE dlib::dlib)
# opencv is weird like that
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(vanche PRIVATE ${OpenCV_LIBS})