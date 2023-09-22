include_directories(include/)

# file(COPY ../models DESTINATION ${CMAKE_BINARY_DIR})
# file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})


SET(VANCHE_SOURCES
 renderer/window.c
 renderer/vmodelRenderer.cpp
 loader/loader.cpp
 loader/model.cpp)

add_executable(vanche main.cpp ${VANCHE_SOURCES})

target_link_libraries(vanche PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(vanche PRIVATE glad)
# target_link_libraries(vanche PRIVATE OpenGL::GL)
target_link_libraries(vanche PRIVATE glfw)
target_link_libraries(vanche PRIVATE glm::glm)
target_link_libraries(vanche PRIVATE nlohmann_json::nlohmann_json)