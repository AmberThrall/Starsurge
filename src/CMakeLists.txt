add_library(Starsurge STATIC Engine.cpp Game.cpp)
target_include_directories(Starsurge PUBLIC ${PROJECT_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIR} ${GLFW3_INCLUDE_DIR})
target_link_libraries(Starsurge ${OPENGL_gl_LIBRARY} ${GLFW3_LIBRARY})
target_include_directories(Starsurge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
