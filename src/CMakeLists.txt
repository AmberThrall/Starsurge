add_library(Starsurge STATIC Logging.cpp
    Game.cpp
    glad.c
    Vector.cpp
    Color.cpp
    Scene.cpp
    Entity.cpp
    Component.cpp
    Mesh.cpp
    Shader.cpp
    BasicShader.cpp
    Material.cpp
    MeshRenderer.cpp
    Utils.cpp
)
target_include_directories(Starsurge PUBLIC ${PROJECT_SOURCE_DIR}/include ${OPENGL_INCLUDE_DIR} ${GLFW3_INCLUDE_DIR})
target_link_libraries(Starsurge ${OPENGL_gl_LIBRARY} ${GLFW3_LIBRARY})
target_include_directories(Starsurge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
