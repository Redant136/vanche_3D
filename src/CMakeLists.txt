include_directories(include/)

find_package(OpenCV)

SET(VANCHE_SOURCES
  renderer/window.c
  renderer/vmodelRenderer.cpp
  loader/vanche_loader.cpp
  physics/vmodelPhysics.cpp
  face_pose_detection/face_recognizerThread.cpp
  face_pose_detection/face_mapper.cpp)
SET(VANCHE_FACE_MAPPER_SOURCES
  face_pose_detection/face_recognizerThread.cpp
  )

OPTION(VANCHE_BUILD_FACE_RECOGNIZER "To build face recognizer" TRUE)

# enable default face engine
IF(NOT DEFINED VANCHE_FACE_ENGINE_dlib68)
  add_definitions(-DVANCHE_FACE_ENGINE_dlib68=1)
  SET(VANCHE_FACE_ENGINE_dlib68 TRUE)
ENDIF()

# face engine restrictions
IF(DEFINED VANCHE_FACE_ENGINE_dlib68)
  IF(NOT OpenCV_FOUND)
    SET(VANCHE_BUILD_FACE_RECOGNIZER FALSE)
    message("Could not find OpenCV required for chosen engine, building without face recognition")
  ENDIF()
ENDIF()


IF(VANCHE_BUILD_FACE_RECOGNIZER)
  IF(DEFINED VANCHE_FACE_ENGINE_dlib68)
    SET(VANCHE_SOURCES ${VANCHE_SOURCES} face_pose_detection/face_recognizer_dlib68.cpp)
    SET(VANCHE_FACE_MAPPER_SOURCES ${VANCHE_FACE_MAPPER_SOURCES} face_pose_detection/face_recognizer_dlib68.cpp)
  ENDIF()
  
  add_executable(face_mapper face_pose_detection/face_mapper_configurator.cpp ${VANCHE_FACE_MAPPER_SOURCES})
ENDIF()

add_executable(vanche main.cpp ${VANCHE_SOURCES})

target_link_libraries(vanche PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(vanche PRIVATE glad)
target_link_libraries(vanche PRIVATE glfw)
target_link_libraries(vanche PRIVATE glm::glm)
target_link_libraries(vanche PRIVATE nlohmann_json::nlohmann_json)


IF(VANCHE_BUILD_FACE_RECOGNIZER)
  IF(DEFINED VANCHE_FACE_ENGINE_dlib68)
    include_directories(vanche PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(vanche PRIVATE ${OpenCV_LIBS})
    include_directories(face_mapper PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(face_mapper PRIVATE ${OpenCV_LIBS})
  ENDIF()

  target_link_libraries(vanche PRIVATE dlib::dlib)
  target_link_libraries(face_mapper PRIVATE ${CMAKE_DL_LIBS})
  target_link_libraries(face_mapper PRIVATE glm::glm)
  target_link_libraries(face_mapper PRIVATE dlib::dlib)
ENDIF()